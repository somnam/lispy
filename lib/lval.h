#ifndef lval_h
#define lval_h

// Declare Lisp Value struct.
typedef struct {
    int type;
    int err;
    double num;
} lval;

// Possible lval types.
enum { LVAL_NUM, LVAL_ERR };

// Possible error types.
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

extern lval lval_num(double x);
extern lval lval_err(int x);
extern void lval_print(lval v);
extern void lval_println(lval v);

#endif
