#include <stdio.h>

#include "lval.h"

// Create lval instances.
lval lval_num(double x) {
    lval v;
    v.type = LVAL_NUM;
    v.num  = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err  = x;
    return v;
}

// Print an lval.
void lval_print(lval v) {

    switch (v.type) {
        // Print number values.
        case LVAL_NUM:
            printf("%f\n", v.num);
        break;
        // Handle error.
        case LVAL_ERR:
        {
            // Check error type.
            char* message;
            if (v.err == LERR_DIV_ZERO) {
                message = "Division by zero.";
            } else if (v.err == LERR_BAD_OP) {
                message = "Invalid operator.";
            } else if (v.err == LERR_BAD_NUM) {
                message = "Invalid number.";
            }
            printf("Error: %s", message);
            break;
        }
    }
}

// Print an lval followed by a newline.
void lval_println(lval v) { lval_print(v); putchar('\n'); }

