#ifndef LISPFUNCTIONSLIB_H_INCLUDED
#define LISPFUNCTIONSLIB_H_INCLUDED



#endif // LISPFUNCTIONSLIB_H_INCLUDED

#define LASSERT(args, cond, fmt, ...) \
 if (!(cond)) { \
    lval* err = lval_err(fmt, ##__VA_ARGS__); \
    lval_del(args); \
    return err; \
 }
#define LARGNUM(arg,count, cond, fmt, ...) \
    if(count != cond) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(arg);    \
        return err;\
    }
#define ISEMPTY(arg, count, fmt, ...)\
    if(count == 0) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(arg);    \
        return err;\
    }
#define LTYPE(arg, type, cond, fmt, ...) \
    if(type != cond) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(arg);    \
        return err;\
    }

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
    lbuiltin fun;

    int count;
    lval** cell;
};

struct lenv {
    int count;
    char** syms;
    lval** vals;
};

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
lval* lval_push(lval* v, lval* x, int i);
lval* lval_take(lval* v, int i);
lval* lval_copy(lval* v);
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
lval* lval_join(lval* x, lval* y);
lval* builtin(lenv* e,lval* a, char* func);

/*Create Enumeration of possible lval types*/
enum {LVAL_NUM, LVAL_ERR, LVAL_FP_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR,LVAL_FUN};

char* ltype_name(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_FP_NUM: return "Floating point number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    default: return "Unknown";
  }
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {
 /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "%", builtin_mod);
  lenv_add_builtin(e, "^", builtin_pow);
  lenv_add_builtin(e, "min", builtin_min);
  lenv_add_builtin(e, "max", builtin_max);

  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "cons", builtin_cons);
  lenv_add_builtin(e, "init", builtin_init);
  lenv_add_builtin(e, "len", builtin_len);
   /*Variable Functions*/
  lenv_add_builtin(e, "def",  builtin_def);
  lenv_add_builtin(e, "var", builtin_print_vars);
    return;
}

lval* builtin_print_vars(lenv* e, lval* a) {
    for(int i = 0; i < e->count; i++) {
        printf("%s\n", e->syms[i]);
    }
}

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}
char* lenv_get_name(lenv* e, lval* v) {
    for (int i = 0; i < e->count; i++) {
        if ( e->vals[i]->fun == v->fun) {
          return e->syms[i];
        }
    }
    return "No matching function found!\n";
}

lval* lenv_get(lenv* e, lval* k) {

  /* Iterate over all items in environment */
  for (int i = 0; i < e->count; i++) {
    /* Check if the stored string matches the symbol string */
    /* If it does, return a copy of the value */
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }
  /* If no symbol found return error */
  return lval_err("Unbound Symbol '%s'", k->sym);
}

lval* lenv_put(lenv* e, lval* k, lval* v) {

  /* Iterate over all items in environment */
  /* This is to see if variable already exists */

  for (int i = 0; i < e->count; i++) {
    /* If variable is found delete item at that position */
    /* And replace with variable supplied by user */
    if (strcmp(e->syms[i], k->sym) == 0) {
      LASSERT(v, v->type == LVAL_FUN, "Name already belongs to function.");
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }

  /* If no existing entry found allocate space for new entry */
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);

  /* Copy contents of lval and symbol string into new location */
  e->vals[e->count-1] = lval_copy(v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

/* Construct a pointer to a new Floating Point Number lval */
lval* lval_fp_num(double x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FP_NUM;
    v->fp_num = x;
    return v;
}
/* Construct a pointer to a new Number lval */
lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}
/* Construct a pointer to a new Error lval */
lval* lval_err(char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;

  /* Create a va list and initialize it */
  va_list va;
  va_start(va, fmt);

  /* Allocate 512 bytes of space */
  v->err = malloc(512);

  /* printf the error string with a maximum of 511 characters */
  vsnprintf(v->err, 511, fmt, va);

  /* Reallocate to number of bytes actually used */
  v->err = realloc(v->err, strlen(v->err)+1);

  /* Cleanup our va list */
  va_end(va);

  return v;
}

lval* lval_fun(lbuiltin func) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
}

/* Construct a pointer to a new Symbol lval */
lval* lval_sym(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

/* A pointer to a new empty Sexpr lval */
lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

void lval_del(lval* v) {

  switch (v->type) {
    /* Do nothing special for number type */
    case LVAL_FUN: break;
    case LVAL_NUM: break;
    case LVAL_FP_NUM: break;
    /* For Err or Sym free the string data */
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;

    /* If Qexpr or Sexpr then delete all elements inside */
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      /* Also free the memory allocated to contain the pointers */
      free(v->cell);
    break;
  }

  /* Free the memory allocated for the "lval" struct itself */
  free(v);
}


lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ?
    lval_num(x) : lval_err("invalid number");
}

lval* lval_read_fp_num(mpc_ast_t* t, int pos) {
    long x = strtol(t->children[pos]->contents, NULL, 10);
    int len = strlen(t->children[pos+2]->contents);
    long y = strtol(t->children[pos+2]->contents, NULL, 10);
    double v;
    v = x + ((double) y) / pow(10, len);
    return lval_fp_num(v);
}

lval* lval_read(mpc_ast_t* t) {

  /* If Symbol or Number return conversion to that type */
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

  /* If root (>) or sexpr then create empty list */
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }

  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    if (strcmp(t->children[i+1]->contents, ".") == 0) {
        x = lval_add(x, lval_read_fp_num(t, i));
        i += 2;
    }
    else { x = lval_add(x, lval_read(t->children[i])); }
  }

  return x;
}

lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}

/* Print an "lval" */
void lval_print(lenv* e, lval* v) {
  switch (v->type) {
    case LVAL_FUN:
        if(v->fun == builtin_print_vars){builtin_print_vars(e, v);}
        printf("<%s>", lenv_get_name(e,v)  );
        break;
    case LVAL_NUM:   printf("%li", v->num); break;
    case LVAL_FP_NUM:   printf("%f", v->fp_num); break;
    case LVAL_ERR:   printf("Error: %s", v->err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(e, v, '(', ')'); break;
    case LVAL_QEXPR: lval_expr_print(e, v,'{', '}'); break;
  }
}

void lval_expr_print(lenv* e, lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {

    /* Print Value contained within */
    lval_print(e, v->cell[i]);

    /* Don't print trailing space if last element */
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

/* Print an "lval" followed by a newline */
void lval_println(lenv* e, lval* v) { lval_print(e, v); putchar('\n'); }



lval* lval_eval(lenv* e, lval* v) {
  if (v->type == LVAL_SYM) {
    lval* x = lenv_get(e, v);
    lval_del(v);
    return x;
  }
  /* Evaluate Sexpressions */
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
  /* All other lval types remain the same */
  return v;
}

lval* lval_eval_sexpr(lenv* e, lval* v) {

  /* Evaluate Children */
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(e, v->cell[i]);
  }

  /* Error Checking */
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }

  /* Empty Expression */
  if (v->count == 0) { return v; }

  /* Single Expression */
  if (v->count == 1) { return lval_take(v, 0); }

  /* Ensure First Element is Symbol */
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_FUN) {
    lval_del(v); lval_del(f);
    return lval_err("First element is not a function.");
  }

  /* Call builtin with operator */
  lval* result = f->fun(e,v);
  lval_del(f);
  return result;
}


lval* lval_pop(lval* v, int i) {
  /* Find the item at "i" */
  lval* x = v->cell[i];

  /* Shift memory after the item at "i" over the top */
  memmove(&v->cell[i], &v->cell[i+1],
    sizeof(lval*) * (v->count-i-1));

  /* Decrease the count of items in the list */
  v->count--;

  /* Reallocate the memory used */
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

lval* lval_copy(lval* v) {
    lval* x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type) {

    /* Copy Functions and Numbers Directly */
        case LVAL_FUN: x->fun = v->fun; break;
        case LVAL_NUM: x->num = v->num; break;

    /* Copy Strings using malloc and strcpy */
        case LVAL_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err); break;

        case LVAL_SYM:
      x->sym = malloc(strlen(v->sym) + 1);
      strcpy(x->sym, v->sym); break;

    /* Copy Lists by copying each sub-expression */
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
        break;
  }

  return x;
}

lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_mod(lenv* e, lval* a) {
    return builtin_op(e, a, "%");
}

lval* builtin_min(lenv* e, lval* a) {
    return builtin_op(e, a, "min");
}

lval* builtin_max(lenv* e, lval* a) {
    return builtin_op(e, a, "max");
}

lval* builtin_pow(lenv* e, lval* a) {
    return builtin_op(e, a, "^");
}


lval* builtin_op(lenv* e, lval* a, char* op) {

  /* Ensure all arguments are numbers */
  for (int i = 0; i < a->count; i++) {
    /*if (a->cell[i]->type != LVAL_NUM && a->cell[i]->type != LVAL_FP_NUM) {
      lval_del(a);
      return lval_err("Cannot operate on non-number!");
    }*/
    LASSERT(a, a->cell[i]->type == LVAL_NUM || a->cell[i]->type == LVAL_FP_NUM,
            "Expected number, got %s instead", ltype_name(a->cell[i]->type));
  }

  /* Pop the first element */
  lval* x = lval_pop(a, 0);

  /* If no arguments and sub then perform unary negation */
  if ((strcmp(op, "-") == 0) && a->count == 0) {
    if(x->type == LVAL_NUM) {
        x->num = -x->num;
    }
    else { x->fp_num = -x->fp_num; };
  }

  /* While there are still elements remaining */
  while (a->count > 0) {

    /* Pop the next element */
    lval* y = lval_pop(a, 0);
    if(x->type == LVAL_FP_NUM || y->type == LVAL_FP_NUM){
        if(x->type == LVAL_NUM){
                x->type = LVAL_FP_NUM;
                x->fp_num = x->num;
        }

        if (strcmp(op, "+") == 0) {
                if(y->type == LVAL_NUM){ y->fp_num = y->num;}
                x->fp_num += y->fp_num;
        }
        if (strcmp(op, "-") == 0) {
                if(y->type == LVAL_NUM){ y->fp_num = y->num;}
                x->fp_num -= y->fp_num;
        }
        if (strcmp(op, "*") == 0) {
                if(y->type == LVAL_NUM){ y->fp_num = y->num;}
                x->fp_num *= y->fp_num;
        }
        if (strcmp(op, "/") == 0) {
          if(y->type == LVAL_NUM){ y->fp_num = y->num;}
          if (y->fp_num < 1e-12) {
            lval_del(x); lval_del(y);
            x = lval_err("Division By Zero!"); break;
          }
          x->fp_num /= y->fp_num;
        }
        if(strcmp(op, "%") == 0) {
            lval_del(x); lval_del(y);
            x = lval_err("Operation only possible for integer values!"); break;
        }
        if (strcmp(op, "^") == 0){
            if(y->type == LVAL_FP_NUM) { y->num = y->fp_num; }
            double p = x->fp_num;
            for(int i = 1; i < y->num;i++) {
                x->fp_num *= p;
            }
        }
        if (strcmp(op, "min") == 0){
            if(y->type == LVAL_NUM) {
                y->fp_num = y->num;
            }
            if(y->fp_num < x->fp_num){ x->fp_num = y->fp_num; }
        }
        if (strcmp(op, "max") == 0){
            if(y->type == LVAL_NUM) {
                y->fp_num = y->num;
            }
            if(y->fp_num > x->fp_num){ x->fp_num = y->fp_num; }
        }
    }
    else{
        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
          if (y->num == 0) {
            lval_del(x); lval_del(y);
            x = lval_err("Division By Zero!"); break;
          }
          x->num /= y->num;
        }
        if(strcmp(op, "%") == 0) {
            if (y->num == 0) {
            lval_del(x); lval_del(y);
            x = lval_err("Division By Zero!"); break;
          }
          x->num %= y->num;
        }
        if (strcmp(op, "^") == 0){
            long p = x->num;
            for(int i = 1; i < y->num;i++) {
                x->num *= p;
            }
        }
        if (strcmp(op, "min") == 0){
            if(y->num < x->num){ x->num = y->num; }
        }
        if (strcmp(op, "max") == 0){
            if(y->num > x->num){ x->num = y->num; }
        }
    }
    lval_del(y);
  }

  lval_del(a); return x;
}

lval* builtin_head(lenv* e, lval* a) {
  LARGNUM(a, a->count, 1,
    "Function 'head' passed too many arguments."
    "Expected %i, got %i", 1, a->count);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'head' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  ISEMPTY(a,a->count,
    "Function 'head' passed {}!");

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LARGNUM(a, a->count,1,
   "Function 'head' passed too many arguments."
    "Expected %i, got %i", 1, a->count);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'tail' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  ISEMPTY(a,a->count,
    "Function 'tail' passed {}!");

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_list(lenv* e,lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lenv* e, lval* a) {
  LARGNUM(a, a->count,1,
    "Function 'eval' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'eval' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {

  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'join' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  }

  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);
  return x;
}

lval* lval_join(lval* x, lval* y) {
    LASSERT(x, x->cell[0]->type != LVAL_QEXPR,
    "Function 'join' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(x->type), ltype_name(LVAL_QEXPR));
    LASSERT(y, y->cell[0]->type != LVAL_QEXPR,
    "Function 'join' passed incorrect type for argument 1. Got %s, expected %s",
    ltype_name(y->type), ltype_name(LVAL_QEXPR));
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

lval* lval_push(lval* a, lval* x, int i) {
    lval* v = a->cell[i];
    a->count++;
    realloc(a->cell, sizeof(lval*)*a->count);
    memmove(&a->cell[i+1], &a->cell[i], sizeof(lval*) * (a->count-i));
    a->cell[i] = x;
    return v;
}

lval* builtin(lenv* e, lval* a, char* func) {
  if (strcmp("list", func) == 0) { return builtin_list(e, a); }
  if (strcmp("head", func) == 0) { return builtin_head(e, a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(e, a); }
  if (strcmp("join", func) == 0) { return builtin_join(e, a); }
  if (strcmp("eval", func) == 0) { return builtin_eval(e, a); }
  if (strcmp("cons", func) == 0) { return builtin_cons(e, a); }
  if (strcmp("len", func) == 0) { return builtin_len(e, a); }
  if (strcmp("init", func) == 0) { return builtin_init(e, a); }
  if (strstr("+-/*^%minmax", func)) { return builtin_op(e, a, func); }
  lval_del(a);
  return lval_err("Unknown Function!");
}

lval* builtin_cons(lenv* e, lval* a) {
    LARGNUM(a, a->count,2, "Function \"cons\" passed wrong number of arguments");
    LASSERT(a, a->cell[0]->type == LVAL_NUM || a->cell[0]->type == LVAL_FP_NUM,
           "Value is not integer/float. Got %s instead.", ltype_name(a->cell[0]->type));
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR,
      "Function 'cons' passed incorrect type. Expected %s, got %s instead.",
      ltype_name(LVAL_QEXPR), ltype_name(a->cell[1]->type));
    lval* x;
    /* if(a->cell[1]->type == LVAL_NUM){
        x = lval_num(a->cell[0]->num);
    }
    else if(a->cell[1]->type == LVAL_FP_NUM) {
        x = lval_fp_num(a->cell[0]->fp_num);
    }*/
    if(a->cell[0]->type == LVAL_NUM){
        x = lval_num(a->cell[0]->num);
    }
    else if(a->cell[0]->type == LVAL_FP_NUM) {
        x = lval_fp_num(a->cell[0]->fp_num);
    }
    lval_pop(a, 0);
    lval_push(a, x, 0);
    a->type = LVAL_QEXPR;
    lval_del(x);
    return a;
}

lval* builtin_len(lenv* e, lval* a) {
    return lval_num(a->cell[0]->count);
}

lval* builtin_init(lenv* e, lval* a) {
    lval* x = a->cell[0];
    a->cell[0]->count--;
    realloc(a->cell, sizeof(lval*) * a->cell[0]->count);
    return x;
}

lval* builtin_def(lenv* e, lval* a) {
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
      "Function 'def' passed incorrect type. Expected %s, got %s instead.",
      ltype_name(LVAL_QEXPR), ltype_name(a->cell[0]->type));

  /* First argument is symbol list */
  lval* syms = a->cell[0];

  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM,
      "Function 'def' cannot define non-symbol: %s", syms->cell[i]->sym);
  }

  /* Check correct number of symbols and values */
  LASSERT(a, syms->count == a->count-1,
    "Function 'def' cannot define incorrect "
    "number of values to symbols");

  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i+1]);
  }

  lval_del(a);
  return lval_sexpr();
}
