%token IDENT
%token LT GT RSHIFT

%code requires {
  typedef char * string_t;
}

%union {
  string_t sval;
}

%code {
  #include <stdio.h>

  int unmatched_open_bracket = 0;
}

%code provides {
  extern int unmatched_open_bracket;

  int yylex(void);
  void yyerror(const char *msg);
}

%%

input:
    %empty
  | input line  { printf("\n=> "); }
  ;

line:
    '\n'
  | type '\n'
  | error '\n'  { yyerrok; }
  ;

type:
    IDENT       {
                  printf("%s", $<sval>1);
                }
  | IDENT       { printf("%s [ ", $<sval>1); }
      LT        { unmatched_open_bracket++; }
      type GT   {
                  unmatched_open_bracket--;
                  printf(" ]");
                }
  ;

%%

int main(void) {
  printf("=> ");
  return yyparse();
}

void yyerror(const char *msg) {
  fprintf(stderr, "%s\n", msg);
}
