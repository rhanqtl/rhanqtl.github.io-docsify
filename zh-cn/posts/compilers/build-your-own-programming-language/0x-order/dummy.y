%{
  #include <stdio.h>
  #include <stdint.h>
  #include <stdlib.h>

  #define FUCK_IT \
      do { puts("fuck it"); } while (0)
%}


%define api.value.type union

%locations

%token <int> X
%nterm <int> start


%%

start : X ;

%%

static void print_token(yytoken_kind_t kind, YYSTYPE val) {
  if (kind == X) {
    printf("token of kind X, with value %ld\n", val.ival);
  } else {
    fprintf(stderr, "unknown token kind %d\n", kind);
    abort();
  }
}

