#ifndef BUILTINS_H_INCLUDED
#define BUILTINS_H_INCLUDED



#endif // BUILTINS_H_INCLUDED
//built-in function prototypes
lval* builtin_def(lenv* e, lval* a);
lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_mod(lenv* e, lval* a);
lval* builtin_pow(lenv* e, lval* a);
lval* builtin_min(lenv* e, lval* a);
lval* builtin_max(lenv* e, lval* a);
lval* builtin_head(lenv* e,lval* a);
lval* builtin_tail(lenv* e,lval* a);
lval* builtin_list(lenv* e,lval* a);
lval* builtin_eval(lenv* e,lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_len(lenv* e,lval* a);
lval* builtin_init(lenv* e,lval* a);
lval* builtin_cons(lenv* e,lval* a);
lval* builtin_print_vars(lenv* e, lval* a);
lval* builtin(lenv* e,lval* a, char* func);
lval* builtin_exit(lenv* e, lval* a);
