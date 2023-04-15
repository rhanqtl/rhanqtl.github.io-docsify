%code top {
  #include <stdint.h>
}

%union {
  int64_t ival;
  char sval[1024];
}

%token <ival> NUMBER
%token <sval> STRING
%token <sval> FLAG_SHORT_NAME FLAG_LONG_NAME OPT_SHORT_NAME OPT_LONG_NAME

%code {
  #include <stdio.h>

  int yylex(void);
  void yyerror(const char *msg);
}

%%

components:
    %empty
  | components comp
  ;

comp:
    opt
  | arg
  | flag
  ;

opt:
    OPT_SHORT_NAME arg      {
                          printf("OPTION %s\n", $1);
                        }
  | OPT_LONG_NAME '=' arg   {
                          printf("OPTION %s\n", $1);
                        }
  ;

arg:
    STRING  {
              printf("ARGUMENT %s\n", $1);
            }
  | NUMBER  {
              printf("ARGUMENT %d\n", $1);
            }
  ;

flag:
    FLAG_SHORT_NAME      {
                      printf("FLAG %s\n", $1);
                    }
  | FLAG_LONG_NAME       {
                      printf("FLAG %s\n", $1);
                    }
  ;

%%

int main() {
  printf("=> ");
  return yyparse();
}

void yyerror(const char *msg) {
  fprintf(stderr, "%s\n", msg);
}
