#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpc.h"
#include "LispTypes.h"
#include "builtins.h"
#include "LispFunctionsLib.h"



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
  if (strstr("exit", func)) { return builtin_exit(e, a);}
  lval_del(a);
  return lval_err("Unknown Function!");
}

lval* builtin_cons(lenv* e, lval* a) {
    LARGNUM("cons", a, 2);
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
  LARGNUM("head", a, 1);

  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'head' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  ISEMPTY("head", a);

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LARGNUM("tail", a, 1);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'tail' passed incorrect type for argument 0. Got %s, expected %s",
    ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  ISEMPTY("tail", a);

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_list(lenv* e,lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}
lval* builtin_eval(lenv* e, lval* a) {
  LARGNUM("eval", a,1);
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

lval* builtin_print_vars(lenv* e, lval* a) {
    for(int i = 0; i < e->count; i++) {
        printf("%s\n", e->syms[i]);
    }
}

lval* builtin_exit(lenv* e, lval* a) {
    return a;
}
