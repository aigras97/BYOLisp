#ifndef LISPTYPES_H_INCLUDED
#define LISPTYPES_H_INCLUDED



#endif // LISPTYPES_H_INCLUDED

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
    int type;
    union {
        double fp_num;
        long num;
        char* err;
    };
    char* sym;
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;

    int count;
    lval** cell;
};

struct lenv {
    lenv* par;
    int count;
    char** syms;
    lval** vals;
};

enum {LVAL_NUM, LVAL_ERR, LVAL_FP_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR,LVAL_FUN};
