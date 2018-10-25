#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpc.h"
#include "LispTypes.h"
#include "builtins.h"
#include "LispFunctionsLib.h"
#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif






int main(int argc, char** argv) {

  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Qexpr = mpc_new("qexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");


  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ |'.'|/[0-9]+/;                             \
      symbol :  /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%^]+/ ;               \
      sexpr    : '(' <expr>* ')' ;                      \
      qexpr    : '{' <expr>* '}' ;                  \
      expr     : <number> | <symbol> | <sexpr>| <qexpr>;  \
      lispy    : /^/ <expr>* /$/ ;             \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

  puts("Lispy Version 0.0.0.0.5");
  puts("Press Ctrl+c to Exit\n");

  lenv* e = lenv_new();
  lenv_add_builtins(e);
  while (1) {

    char* input = readline("lispy> ");
    add_history(input);
    lval* x;
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {

      x = lval_eval(e ,lval_read(r.output));
      lval_println(e, x);

      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    if(x->type == LVAL_FUN && x->fun == builtin_exit){
         lval_del(x);
         free(input);
         lenv_del(e);
         mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
         return 0;
    }
    lval_del(x);
    free(input);

  }
  lenv_del(e);
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

  return 0;
}
