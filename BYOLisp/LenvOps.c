#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpc.h"
#include "LispTypes.h"
#include "builtins.h"
#include "LispFunctionsLib.h"

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
  lenv_add_builtin(e, "exit", builtin_exit);
    return;
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
