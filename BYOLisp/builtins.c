#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
 return builtin_var(e, a, "def");
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

lval* builtin_lambda(lenv* e, lval* a) {
  /* Check Two arguments, each of which are Q-Expressions */
  LARGNUM("\\", a, 2);
  LTYPE("\\", a->cell[0],  LVAL_QEXPR);
  LTYPE("\\", a->cell[1],  LVAL_QEXPR);

  /* Check first Q-Expression contains only Symbols */
  for (int i = 0; i < a->cell[0]->count; i++) {
    LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
      "Cannot define non-symbol. Got %s, Expected %s.",
      ltype_name(a->cell[0]->cell[i]->type),ltype_name(LVAL_SYM));
  }

  /* Pop first two arguments and pass them to lval_lambda */
  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);

  return lval_lambda(formals, body);
}

lval* builtin_var(lenv* e, lval* a, char* func) {
  LTYPE(func, a->cell[0], LVAL_QEXPR);

  lval* syms = a->cell[0];
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
      "Function '%s' cannot define non-symbol. "
      "Got %s, Expected %s.", func,
      ltype_name(syms->cell[i]->type),
      ltype_name(LVAL_SYM));
  }

  LASSERT(a, (syms->count == a->count-1),
    "Function '%s' passed too many arguments for symbols. "
    "Got %i, Expected %i.", func, syms->count, a->count-1);

  for (int i = 0; i < syms->count; i++) {
    /* If 'def' define in globally. If 'put' define in locally */
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }

    if (strcmp(func, "=")   == 0) {
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    }
  }

  lval_del(a);
  return lval_sexpr();
}

lval* builtin_put(lenv* e, lval* a) {
  return builtin_var(e, a, "=");
}

lval* builtin_fun(lenv* e, lval* a) {
    LARGNUM("fun", a, 2);
    LTYPE("fun", a->cell[0], LVAL_QEXPR);
    LTYPE("fun", a->cell[1], LVAL_QEXPR);

    //separate function name and argument list
    lval* k = lval_pop(a, 0);
    lval* j = lval_pop(k, 0);
    lval* v = lval_qexpr();

    v->cell = malloc(sizeof(lval*));
    v->cell[0] = j;
    v->count++;
    lval* f = lval_qexpr();
    f->cell = malloc(sizeof(lval*));
    f->cell[0] = lval_lambda(k, a->cell[0]);
    f->count++;
    lval_push(f, v, 0);
    return builtin_def(e, f);
}

lval* builtin_ord(lenv* e, lval* a, char* func) {
    LARGNUM(func, a, 2);
    LASSERT(a, a->cell[0]->type == LVAL_NUM || a->cell[0]->type == LVAL_FP_NUM, "Function"
    " %s expected %s or %s, got %s.", func,ltype_name(LVAL_NUM),
    ltype_name(LVAL_FP_NUM), ltype_name(a->cell[0]->type ));
    LASSERT(a, a->cell[1]->type == LVAL_NUM || a->cell[1]->type == LVAL_FP_NUM, "Function"
    " %s expected %s or %s, got %s.", func,ltype_name(LVAL_NUM),
    ltype_name(LVAL_FP_NUM), ltype_name(a->cell[1]->type ));
    if((a->cell[0]->type != a->cell[1]->type) && a->cell[0]->type == LVAL_FP_NUM) {
        a->cell[1]->fp_num = (double) a->cell[1]->num;
        a->cell[1]->type = LVAL_FP_NUM;
    }
    else if((a->cell[0]->type != a->cell[1]->type) && a->cell[0]->type == LVAL_NUM) {
        a->cell[0]->fp_num = (double) a->cell[0]->num;
        a->cell[0]->type = LVAL_FP_NUM;
    }

    if(a->cell[0]->type == LVAL_NUM) {
        if(strcmp(func, ">") == 0) {
            lval* result = lval_bool();
            if(a->cell[0]->num > a->cell[1]->num) {
                result->lbool = true;
                return result;
            }
            else {
                result->lbool = false;
                return result;
            }
        }
        if(strcmp(func, "<") == 0) {
            lval* result = lval_bool();
            if(a->cell[0]->num < a->cell[1]->num) {
                result->lbool = true;
                return result;
            }
            else {
                result->lbool = false;
                return result;
            }
        }
        if(strcmp(func, "==") == 0) {
            lval* result = lval_bool();
            if(a->cell[0]->num == a->cell[1]->num) {
                result->lbool = true;
                return result;
            }
            else {
                result->lbool = false;
                return result;
            }
        }
    }
    else if(a->cell[0]->type == LVAL_FP_NUM) {
        if(strcmp(func, ">") == 0) {
            lval* result = lval_bool();
            if(a->cell[0]->fp_num > a->cell[1]->fp_num) {
                result->lbool = true;
                return result;
            }
            else {
                result->lbool = false;
                return result;
            }
        }
        if(strcmp(func, "<") == 0) {
            lval* result = lval_bool();
            if(a->cell[0]->fp_num < a->cell[1]->fp_num) {
                result->lbool = true;
                return result;
            }
            else {
                result->lbool = false;
                return result;
            }
        }
        if(strcmp(func, "==") == 0) {
            lval* result = lval_bool();
            if(a->cell[0]->fp_num == a->cell[1]->fp_num) {
                result->lbool = true;
                return result;
            }
            else {
                result->lbool = false;
                return result;
            }
        }
    }
}

lval* builtin_gt(lenv* e, lval* a) {
    return builtin_ord(e, a, ">");
}
lval* builtin_lt(lenv* e, lval* a) {
    return builtin_ord(e, a, "<");
}
lval* builtin_eq(lenv* e, lval* a) {
    return builtin_ord(e, a, "==");
}
