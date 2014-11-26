#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <editline/readline.h>
#include <editline/history.h>

#include "lib/mpc.h"
#include "lib/lval.h"

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
            lval* result = lval_eval(lval_read(r.output));
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


