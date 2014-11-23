#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lval.h"
#include "mpc.h"

// Create lval s-expression pointer.
lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->len  = 0;
    v->cell = NULL;
    return v;
}

// Create lval operator pointer.
lval* lval_op(char *op) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_OP;
    v->op   = malloc(strlen(op) + 1);
    strcpy(v->op, op);
    return v;
}

// Create lval value pointer.
lval* lval_num(double x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num  = x;
    return v;
}

// Create lval error pointer.
lval* lval_err(int x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err  = x;
    return v;
}

// Destruct lval.
void lval_del(lval* v) {
    switch (v->type) {
        // Do nothing special for number and error types.
        case LVAL_NUM:
        case LVAL_ERR:
            break;
        // Free string data.
        case LVAL_OP:
            free(v->op);
            break;
        case LVAL_SEXPR:
            // Free each nested lvalue.
            for (int i = 0; i < v->len; i++) {
                lval_del(v->cell[i]);
            }

            // Free memory allocated by the pointers array.
            free(v->cell);
    }

    // Free memory allocated by the lval struct itself.
    free(v);
}

lval* lval_add(lval* v, lval* elem) {
    v->len++;
    v->cell           = realloc(v->cell, sizeof(lval*) * v->len);
    v->cell[v->len-1] = elem;
    return v;
}

lval* lval_read_num(mpc_ast_t* t) {
    // strtod - converts char* to double.
    // t->contents - node value.
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
}

lval* lval_read(mpc_ast_t* t) {

    // Number node - return value.
    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    }

    lval* result = NULL;

    // Operator node - return value.
    if (strstr(t->tag, "operator")) {
        result = lval_op(t->contents);
    // Root or sexpr node.
    } else if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr")) {
        // Create an empty lval.
        result = lval_sexpr();

        // Fill lval list with any valid expression contained within the node.
        for (int i = 0; i < t->children_num; i++) {
            // Skip opening and closing tags and regex nodes.
            char* contents = t->children[i]->contents;
            char* tag      = t->children[i]->tag;
            if (strcmp(contents, "(") == 0
                || strcmp(contents, ")") == 0
                || strcmp(contents, "{") == 0
                || strcmp(contents, "}") == 0
                || strcmp(tag, "regex")  == 0
            ) {
                continue;
            }

            // Append lval to list.
            result = lval_add(result, lval_read(t->children[i]));
        }
    }

    return result;
}

void lval_sexpr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->len; i++) {
        // Print cell value.
        lval_print(v->cell[i]);

        // Don't print trailing space for last element.
        if (i != v->len-1) {
            putchar(' ');
        }
    }
    putchar(close);
}

// Print an lval.
void lval_print(lval* v) {

    switch (v->type) {
        // Print number values.
        case LVAL_NUM:
            printf("%f", v->num);
            break;
        // Handle error.
        case LVAL_ERR:
        {
            // Check error type.
            char* message;
            if (v->err == LERR_DIV_ZERO) {
                message = "Division by zero.";
            } else if (v->err == LERR_BAD_OP) {
                message = "Invalid operator.";
            } else if (v->err == LERR_BAD_NUM) {
                message = "Invalid number.";
            }
            printf("Error: %s", message);
            break;
        }
        case LVAL_OP:
            printf("%s", v->op);
            break;
        case LVAL_SEXPR:
            lval_sexpr_print(v, '(', ')');
            break;
    }
}

// Print an lval followed by a newline.
void lval_println(lval* v) { lval_print(v); putchar('\n'); }

