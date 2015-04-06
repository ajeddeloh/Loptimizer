#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "gate.h"

static bool validate_operation(const char *operation, size_t n_inputs);

static bool validate_operation(const char *operation, size_t n_inputs) {
    if(operation == NULL && n_inputs == 0) {
        return true; //input gate
    }
    int stack_size = 0;
    while(*operation != '\0') {
        switch(*operation) {
        case '!':
            if(stack_size == 0) return false;
            break;
        case '+':
        case '|':
        case '&':
        case '*':
        case '^':
            if(stack_size < 2) return false;
            stack_size --;
            break;
        default:
            if(!isupper(*operation)) return false;
            if(*operation - 'A' >= (int)n_inputs) return false;
            stack_size++;
            break;
        }
        operation++;
    }
    return true;
}

Gate *gate_new (const char* name, int n_inputs, const char *operation, size_t n_gates) {
    Gate *gate = malloc( sizeof(Gate) );
    gate->n_inputs = n_inputs;
    if( operation != NULL) {
        gate->operation = strdup(operation);
    } else {
        gate->operation = NULL;
    }
    gate->name = strdup(name);
    gate->n_gates = n_gates;
    return gate;
}

void gate_free(Gate* gate) {
    free(gate->operation);
    free(gate->name);
    free(gate);
}

//gate file format:
//#comments
//n_inputs
//n_gates
//operation string
//optimizer setting (esp. important for 3+ input gates): options none, symmetric, repeatable
//none tries all possible combinations
//symmetric tries all combination assuming the function is symmetric (e.g. F(a,b,c) = F(c,a,b) or
//some other rearrangement)
//repeatable is like symmetric but also assumes if one of the arguments is repeated, it doesn't matter
//which one. (e.g. F(a,a,b) = F(a,b,b)
Gate *gate_parse(char *path) {
    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        perror(path);
        return NULL;
    }
    char *line = NULL;
    size_t n = 0;
    Gate *gate = calloc(1, sizeof(Gate));

    while( getline(&line, &n, fp) != -1) {
        *strchr(line, '\n') = '\0'; //replace newline with null terminator
        if( line[0] == '#' || strlen(line) == 0 ) {
            continue;
        }
        char *key = strtok(line, " =");
        char *value = strtok(NULL, " =");
        
        if( strcmp(key, "name") == 0) {
            gate->name = strdup(value);
        } else if( strcmp(key, "inputs") == 0) {
            gate->n_inputs = atol(value);
        } else if( strcmp(key, "gatesperic") == 0) {
            gate->n_gates = atol(value);
        } else if( strcmp(key, "operation") == 0) {
            gate->operation = strdup(value);
        } else if( strcmp(key, "optimization") == 0) {
            if( strcmp(value, "symmetric") == 0) {
                gate->optimization = GATE_OPT_SYM;
            } else if( strcmp(value, "repeatable") == 0) {
                gate->optimization = GATE_OPT_REP;
            } else if( strcmp(value, "none" ) == 0) {
                gate->optimization = GATE_OPT_NONE;
            } 
        } else {
            printf("Error parsing %s: unknown key %s, ignoring key\n", path, key);
        }           
    }
    free(line);
    fclose(fp);
    
    //check for correctness 

    //was name specified?
    if(gate->name == NULL) {
        printf("Gate name not specified for %s. This gate will be ignored\n", path);
        gate_free(gate);
        return NULL;
    }

    //was number of inputs specified and valid?
    if( gate->n_inputs == 0) {
        printf("Unspecified or invalid number of inputs for %s. This gate will be ignored\n", path);
        gate_free(gate);
        return NULL;
    }

    //was the number of gates per ic specified?
    if( gate->n_gates == 0) {
        printf("Unspecified or invalid number of gates per ic for %s. This gate will be ignored\n", path);
        gate_free(gate);
        return NULL;
    }
    
    if( gate->optimization == 0) {
        printf("Unspecified or invalid optimization behacvior for %s. This gate will be ignored\n", path);
        gate_free(gate);
        return NULL;
    }

    //was the operation specified?
    if( gate->operation == NULL) {
        printf("Unspecified operation for %s. This gate will be ignored\n", path);
        gate_free(gate);
        return NULL;
    }

    //was the operation specified valid postfix?
    if( !validate_operation(gate->operation, gate->n_inputs) ) {
        printf("Invalid operation for %s. This gate will be ignore\n", path);
        gate_free(gate);
        return NULL;
    }
    
    //everything passed its checks, we're all good!
    return gate;
}

//pass NULL for g to get next val, will set indices to NULL and free the indices at the last
//index
//only works with associative gates for now
void gate_generate_indices(Gate *g, size_t **indices, size_t closed_set_size) {
    static size_t pos = 0;

    //starting a new gate
    size_t old_set_size = closed_set_size-1; //does not inc. the new elem
    if(*indices == NULL) {
        *indices = calloc(g->n_inputs, sizeof(size_t));
        pos = 0;
        if(g->optimization != GATE_OPT_REP) {
            (*indices)[pos] = closed_set_size-1;
        } else {
            for(size_t i = 0; i < g->n_inputs; i++) {
                (*indices)[i] = closed_set_size-1;
            }
        }
        return;
    }        
    
    size_t *idxs = *indices;
    //special case for inverters
    if(g->n_inputs == 1) {
        goto free_and_exit;
    }
    //optimization specific below
    switch (g->optimization) {
        case GATE_OPT_SYM: {
            size_t i = g->n_inputs - 1;
            idxs[i] ++; //inc the init idx
            //deal with overflows
            while(true) {
                if (idxs[i] != closed_set_size) return;
                if (i == 1) { 
                    goto free_and_exit;
                }
                idxs[i-1] ++;
                idxs[i] = idxs[i-1];
            }
            break;
        }
        case GATE_OPT_NONE: {
            size_t i = (pos == 0) ? 1 : 0;
            idxs[i] ++; //inc the init idx (not pos)

            //check for overflows
            while (true) {
                //elems before pos cant inc the new elem
                size_t limit = (i < pos) ? old_set_size : closed_set_size;
                if (idxs[i] != limit) return;
                idxs[i] = 0;
                size_t next = (i+1 == pos) ? i+2 : i+1; //skip pos
                if (next == g->n_inputs) { //all possibilities exhausted for this positition
                    pos++;
                    if( pos == g->n_inputs) goto free_and_exit;

                    memset(idxs, 0, g->n_inputs * sizeof(size_t));
                    idxs[pos] = old_set_size; //idx of new elem
                    return;
                }
                i = next;
                idxs[next]++;
            }
            break;
        }
        case GATE_OPT_REP: {
            size_t i = 0;
            idxs[0]--;
            if(idxs[0] + 1 != 0) return; //if no overflow
            idxs[0] = 0;
            while((i+1 != g->n_inputs-1) && (idxs[i+1] == 1 + idxs[i])) { //find the things to be dec'd
                i++;
            }

            i++;
            if ((i == g->n_inputs) || (idxs[i] <= i)) goto free_and_exit;
            idxs[i] --;
            while ( i != 0) {
                idxs[i-1] = idxs[i] - 1;
                i--;
            }
            return;
        }
    }

free_and_exit:
    free(*indices);
    *indices = NULL;
    return;
}

