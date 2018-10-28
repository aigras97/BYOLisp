#ifndef LISPFUNCTIONSLIB_H_INCLUDED
#define LISPFUNCTIONSLIB_H_INCLUDED



#endif // LISPFUNCTIONSLIB_H_INCLUDED

#define LASSERT(args, cond, fmt, ...) \
 if (!(cond)) { \
    lval* err = lval_err(fmt, ##__VA_ARGS__); \
    lval_del(args); \
    return err; \
 }
#define LARGNUM(func,arg, expected) \
    LASSERT(arg, arg->count == expected, "Function %s passed incorrect number of arguments.\
 Expected %i, got %i.", func, expected, arg->count);
#define ISEMPTY(func, arg)\
    LASSERT(arg, arg->count != 0, "Function %s passed {}!", func);
#define LTYPE(func, arg, cond) \
    LASSERT(arg, arg->type == cond, "Function %s passed wrong type. Expected\
%s, got %s", func, cond, arg->type);



// Function declaration list
char* lenv_get_name(lenv* e, lval* v);
char* ltype_name(int t);
lenv* lenv_new(void);
void lenv_del(lenv* e);
lval* lenv_get(lenv* e, lval* k);
lval* lenv_put(lenv* e, lval* k, lval* v);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtins(lenv* e);
lval* lval_num(long x);
lval* lval_fp_num(double x);
lval* lval_sym(char* s);
lval* lval_err(char* fmt, ...);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_fun(lbuiltin func);
void lval_del(lval* v);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_fp_num(mpc_ast_t* t, int pos);
lval* lval_read(mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);
void lval_expr_print(lenv* e, lval* v, char open, char close);
void lval_print(lenv* e, lval* v);
void lval_println(lenv* e, lval* v);
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);
lval* lval_pop(lval* v, int i);
void lval_push(lval* v, lval* x, int i);
lval* lval_take(lval* v, int i);
lval* lval_copy(lval* v);
lval* lval_join(lval* x, lval* y);
lval* lval_lambda(lval* formals, lval* body);
lenv* lenv_copy(lenv* e);
void lenv_def(lenv* e, lval* k, lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);



