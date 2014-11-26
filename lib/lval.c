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

lval* eval_op(lval* a, lval* b, char* op) {
    if (strcmp(op, "+") == 0
        || strcmp(op, "add") == 0
    ) {
        /* x = lval_num(x.num + y.num); */
        a->num += b->num;
    } else if (
        strcmp(op, "-") == 0
        || strcmp(op, "sub") == 0
    ) {
        /* x = lval_num(x.num - y.num); */
        a->num -= b->num;
    } else if (
        strcmp(op, "*") == 0
        || strcmp(op, "mul") == 0
    ) {
        /* x = lval_num(x.num * y.num); */
        a->num *= b->num;
    } else if (
        strcmp(op, "/") == 0
        || strcmp(op, "div") == 0
    ) {
        /* x = y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num); */
        if (b->num == 0) {
            lval_del(a);
            a = lval_err(LERR_DIV_ZERO);
        } else {
            a->num /= b->num;
        }
    } else if (
        strcmp(op, "%") == 0
        || strcmp(op, "mod") == 0
    ) {
        /* x = lval_num(fmod(x.num, y.num)); */
        a->num = fmod(a->num, b->num);
    }
    else if (strcmp(op, "^") == 0) {
        /* x = lval_num(pow(x.num, y.num)); */
        a->num = pow(a->num, b->num);
    }
    else if (strcmp(op, "max") == 0) {
        /* x = x.num > y.num ? x : y; */
        a->num = a->num > b->num ? a->num : b->num ;
    }
    else if (strcmp(op, "min") == 0) {
        /* x = x.num < y.num ? x : y; */
        a->num = a->num < b->num ? a->num : b->num;
    }
    else {
        lval_del(a);
        a = lval_err(LERR_BAD_OP);
        /* x = lval_err(LERR_BAD_OP); */
    }

    return a;
}

lval* eval_single_op(lval* a, char* op) {
    if (strcmp(op, "-") == 0
        || strcmp(op, "sub") == 0
    ) {
        a->num = -a->num;
    }

    return a;
}

lval* lval_op_eval(lval* v, char* op) {
    // Ensure all arguments are numbers.
    for (int i = 0; i < v->len; i++) {
        if (v->cell[i]->type != LVAL_NUM) {
            lval_del(v);
            return lval_err(LERR_BAD_NUM);
        }
    }

    lval *a = NULL, *b = NULL;

    // Iterate over each argument and apply operand.
    while (v->len > 0) {
        // First iteration, assign 'a' to first operand.
        if (!a) {
            a = lval_splice(v, 0);
        }

        // Assign 'b' to next operand, if available.
        if (v->len > 0) {
            b = lval_splice(v, 0);
        }

        if (b) {
            // Evaluate 'a' and 'b' operands.
            a = eval_op(a, b, op);

            // Free 'b'.
            lval_del(b);
        } else {
            // 'a' is the only operand, run single op evaluation.
            a = eval_single_op(a, op);
        }

        // Break loop on error.
        if (a->type != LVAL_NUM) {
            break;
        }
    }

    lval_del(v);

    return a;
}

lval* lval_sexpr_eval(lval* v) {
    // Evaluate children.
    for (int i = 0; i < v->len; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    for (int i = 0; i < v->len; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_splice_del(v, i);
        }
    }

    lval* result = NULL;
    // Empty expression.
    if (v->len == 0) {
        result = v;
    // Single expression.
    } else if (v->len == 1) {
        result = lval_splice_del(v, 0);
    // Multiple expressions.
    } else {
        // Ensure that first element of expression is a symbol.
        lval* first = lval_splice(v, 0);
        if (first->type != LVAL_OP) {
            lval_del(first);
            lval_del(v);
            return lval_err(LERR_BAD_OP);
        }

        // Call operator eval.
        result = lval_op_eval(v, first->op);
        lval_del(first);
    }

    return result;
}

lval* lval_eval(lval* v) {
    // Evaluate S-Expressions. All other types remain the same.
    return (v->type == (LVAL_SEXPR)) ? lval_sexpr_eval(v) : v;
}

lval* lval_splice(lval* v, int i) {
    // Find the item at index.
    lval* item = v->cell[i];

    // Shift memory after the item at i over the top.
    memmove(
        // Destination place in memory, to wich we will copy ...
        &v->cell[i],
        // ... value at next list cell.
        &v->cell[i+1],
        // How much to copy?
        (sizeof(lval*) * (v->len - i - 1))
    );

    // Decrease the count of items in the list.
    v->len--;

    // Reallocate the used memory.
    v->cell = realloc(v->cell, sizeof(lval*) * v->len);

    return item;
}

lval* lval_splice_del(lval* v, int i) {
    lval* res = lval_splice(v, i);
    lval_del(v);
    return res;
}

