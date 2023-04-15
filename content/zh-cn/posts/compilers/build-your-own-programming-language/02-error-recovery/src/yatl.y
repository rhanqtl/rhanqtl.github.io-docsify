%code {
  #include <stdarg.h>
  #include <stdio.h>

  int yylex(void);
  void yyerror(const char *fmt, ...);
}

%union {
  char sval[1024];
}

%token <sval> NUMBER IDENT

%left '+' '-'
%left '*' '/'

%locations

%%

tuple:
    '(' expr_list ')'
  | '(' error ')'
  ;

expr_list:
    expr ','
  | expr_list ',' expr
  | error ','
  | expr_list ',' error
  ;

expr:
    NUMBER
  | expr '+' expr
  | expr '-' expr
  | expr '*' expr
  | expr '/' expr
  ;

%%

int main(void) {
  printf("=> ");
  return yyparse();
}

void yyerror(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

