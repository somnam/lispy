#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

#include "lib/mpc.h"

long eval_op(long x, char* op, long y) {

    if (strcmp(op, "+") == 0
        || strcmp(op, "add") == 0
    ) {
        x += y;
    } else if (
        strcmp(op, "-") == 0
        || strcmp(op, "sub") == 0
    ) {
        x -= y;
    } else if (
        strcmp(op, "*") == 0
        || strcmp(op, "mul") == 0
    ) {
        x *= y;
    } else if (
        strcmp(op, "/") == 0
        || strcmp(op, "div") == 0
    ) {
        x /= y;
    } else if (
        strcmp(op, "%") == 0
        || strcmp(op, "mod") == 0
    ) {
        x %= y;
    }
    else if (strcmp(op, "^") == 0) {
        x = pow(x, y);
    }
    else if (strcmp(op, "max") == 0) {
        x = x > y ? x : y;
    }
    else if (strcmp(op, "min") == 0) {
        x = x < y ? x : y;
    }
    else {
        x = 0;
    }

    return x;
}

long eval_single_op(long x, char* op) { // {{{

    if (strcmp(op, "-") == 0
        || strcmp(op, "sub") == 0
    ) {
        x = -x;
    }

    return x;
} // }}}

int eval(mpc_ast_t* t) {
    /* Evaluate expression node */

    // If node is <number> - return value directly.
    // strstr - check if one string includes another.
    if (strstr(t->tag, "number")) {
        /* printf("Number: %s\n", t->contents); */
        // atoi - converts char* to long.
        // t->contents - node value.
        return atoi(t->contents);
    }

    // Node contains <operator> and <expr>.
    // The operator value is always the second child.
    char* operator = t->children[1]->contents;
    /* printf("operator: %s\n", operator); */

    // Evaluate first <expr> child, so we have a starting point.
    long result = eval(t->children[2]);

    // Iterate remaining expressions.
    int i       = 3;
    if (strstr(t->children[i]->tag, "expr")) {
        while (strstr(t->children[i]->tag, "expr")) {
            result = eval_op(result, operator, eval(t->children[i]));
            i++;
        }
    }
    // No further expressions, evaluate operator in single arguemtn context.
    else {
        result = eval_single_op(result, operator);
    }

    return result;
}

int main(int argc, char** argv) {
    // Create Parsers.
    mpc_parser_t* Number         = mpc_new("number");
    mpc_parser_t* OperatorSymbol = mpc_new("operator_symbol");
    mpc_parser_t* OperatorText   = mpc_new("operator_text");
    mpc_parser_t* Operator       = mpc_new("operator");
    mpc_parser_t* Expr           = mpc_new("expr");
    mpc_parser_t* Lispy          = mpc_new("lispy");

    // Define polish notation language rules.
    mpca_lang(MPCA_LANG_DEFAULT,
        " number            : /-?[0-9]+(\\.[0-9]+)?/ ;"
        " operator_symbol   : '+' | '-' | '*' | '/' | '%' | '^' ;"
        " operator_text     : \"add\" | \"sub\" | \"mul\" | \"div\" | \"mod\" "
        "                   | \"max\" | \"min\" ;"
        " operator          : <operator_symbol> | <operator_text> ;"
        " expr              : <number> | '(' <operator> <expr>+ ')' ;"
        " lispy             : /^/ <operator> <expr>+ /$/ ;",
        Number, OperatorSymbol, OperatorText, Operator, Expr, Lispy
    );

    // Print version and exit information.
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    // Never ending loop.
    while (1) {
        // Output our prompt to get input.
        // The function strips the trailing newline character 
        char* input = readline("lispy> ");

        // Add input to history.
        add_history(input);

        // Attempt to parse the user input.
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        } else {
            // Print the error.
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // Imported from stdlib.h, free retrived input.
        free(input);
    }

    // Delete Parsers.
    mpc_cleanup(6, Number, OperatorSymbol, OperatorText, Operator, Expr, Lispy);

    return 0;
}


