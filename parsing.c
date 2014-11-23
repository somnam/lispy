#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <editline/readline.h>
#include <editline/history.h>

#include "lib/mpc.h"
#include "lib/lval.h"

/*
lval eval_op(lval x, char* op, lval y) { // {{{

    // If either value is an error, return it.
    if (x.type == LVAL_ERR || y.type == LVAL_ERR) {
        return x.type == LVAL_ERR ? x : y;
    }

    if (strcmp(op, "+") == 0
        || strcmp(op, "add") == 0
    ) {
        x = lval_num(x.num + y.num);
    } else if (
        strcmp(op, "-") == 0
        || strcmp(op, "sub") == 0
    ) {
        x = lval_num(x.num - y.num);
    } else if (
        strcmp(op, "*") == 0
        || strcmp(op, "mul") == 0
    ) {
        x = lval_num(x.num * y.num);
    } else if (
        strcmp(op, "/") == 0
        || strcmp(op, "div") == 0
    ) {
        x = y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
    } else if (
        strcmp(op, "%") == 0
        || strcmp(op, "mod") == 0
    ) {
        x = lval_num(fmod(x.num, y.num));
    }
    else if (strcmp(op, "^") == 0) {
        x = lval_num(pow(x.num, y.num));
    }
    else if (strcmp(op, "max") == 0) {
        x = x.num > y.num ? x : y;
    }
    else if (strcmp(op, "min") == 0) {
        x = x.num < y.num ? x : y;
    }
    else {
        x = lval_err(LERR_BAD_OP);
    }

    return x;
} // }}}

lval eval_single_op(lval x, char* op) { // {{{

    if (strcmp(op, "-") == 0
        || strcmp(op, "sub") == 0
    ) {
        x = lval_num(-x.num);
    }

    return x;
} // }}}

lval eval(mpc_ast_t* t) { // {{{
    // Evaluate expression node.

    // If node is <number> - return value directly.
    // strstr - check if one string includes another.
    if (strstr(t->tag, "number")) {
        // strtod - converts char* to double.
        // t->contents - node value.
        errno = 0;
        double x = strtod(t->contents, NULL);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    // Node contains <operator> and <expr>.
    // The operator value is always the second child.
    char* operator = t->children[1]->contents;
    // printf("operator: %s\n", operator);

    // Evaluate first <expr> child, so we have a starting point.
    lval result = eval(t->children[2]);

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
} // }}}
*/

int main(int argc, char** argv) { // {{{
    // Create Parsers.
    mpc_parser_t* Number         = mpc_new("number");
    mpc_parser_t* OperatorSymbol = mpc_new("operator_symbol");
    mpc_parser_t* OperatorText   = mpc_new("operator_text");
    mpc_parser_t* Operator       = mpc_new("operator");
    mpc_parser_t* Sexpr          = mpc_new("sexpr");
    mpc_parser_t* Expr           = mpc_new("expr");
    mpc_parser_t* Lispy          = mpc_new("lispy");

    // Define polish notation language rules.
    mpca_lang(MPCA_LANG_DEFAULT,
        " number            : /-?[0-9]+(\\.[0-9]+)?/ ;"
        " operator_symbol   : '+' | '-' | '*' | '/' | '%' | '^' ;"
        " operator_text     : \"add\" | \"sub\" | \"mul\" | \"div\" | \"mod\" "
        "                   | \"max\" | \"min\" ;"
        " operator          : <operator_symbol> | <operator_text> ;"
        " sexpr             : '(' <expr>* ')' ;"
        " expr              : <number> | <operator> | <sexpr> ;"
        " lispy             : /^/ <expr>* /$/ ;",
        Number, OperatorSymbol, OperatorText, Operator, Sexpr, Expr, Lispy
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
            lval* result = lval_read(r.output);
            lval_println(result);
            lval_del(result);
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
    mpc_cleanup(
        7, 
        Number, OperatorSymbol, OperatorText, Operator, Sexpr, Expr, Lispy
    );

    return 0;
} // }}}


