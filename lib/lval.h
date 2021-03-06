#ifndef lval_h
#define lval_h

#include "mpc.h"

// Declare Lisp Value struct.
typedef struct lval {
    int type;
    double num;
    // Error code.
    int err;
    // S-expression operator.
    char* op;
    // Subexpressions count.
    int len;
    // List of subexpressions.
    struct lval** cell;
} lval;

// Possible lval types.
enum { LVAL_NUM, LVAL_ERR, LVAL_OP, LVAL_SEXPR };

// Possible error types.
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

extern lval* lval_read(mpc_ast_t* t);
extern lval* lval_add(lval* v, lval* elem);
extern void lval_del(lval* v);
extern void lval_print(lval* v);
extern void lval_println(lval* v);
extern lval* lval_eval(lval* v);
extern lval* lval_splice(lval* v, int i);
extern lval* lval_splice_del(lval* v, int i);

#endif
