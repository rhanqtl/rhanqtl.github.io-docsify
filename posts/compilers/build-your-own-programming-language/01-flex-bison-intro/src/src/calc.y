%code requires {
  #include "location.h"
}

%define api.value.type {double}
%define api.location.type {location_t}

%locations

%initial-action {
  yylloc.first_line = yylloc.first_column = 1;
  yylloc.last_line = yylloc.last_column = 1;
};

%define parse.error detailed
%define parse.trace
%define parse.lac full

%code {
  #include <limits.h>
  #include <math.h>
  #include <stdbool.h>
  #include <stdio.h>
  #include <stdarg.h>

  const char *PROMPT = "=> ";
  const char *filename = "<interactive>";
  bool interactive = true;

  extern FILE *yyout;

  int yylex(void);
  void yyerror(const char *fmt, ...);
}

%token NUM

%left '+' '-'
%left '*' '/'
%nonassoc NEG
%right '^'

%%

input : %empty
      | input line { if (interactive) {
                       printf(PROMPT);
                     }
                   }
      ;

line : '\n'
     | expr '\n'  { fprintf(yyout, "%.17g\n", $1); }
     | error '\n' { yyerrok; }
     ;

expr : NUM           { $$ = $1; }
     | '(' expr ')'  { $$ = $2; }
     | expr '+' expr { $$ = $1 + $3; }
     | expr '-' expr { $$ = $1 - $3; }
     | expr '*' expr { $$ = $1 * $3; }
     | expr '/' expr { if ($3) {
                         $$ = $1 / $3;
                       } else {
                         $$ = INT_MIN;
                         fprintf(yyout, "%d:%d-%d:%d: divide by zero\n",
                             @3.first_line, @3.first_column,
                             @3.last_line, @3.last_column);
                       }
                     }
     | expr '^' expr      { $$ = pow($1, $3); }
     | '-' expr %prec NEG { $$ = -$2; }
     ;

%%

int main(int argc, char *argv[]) {
  extern FILE *yyin;

  if (argc > 1) {
    filename = argv[1];
    yyin = fopen(filename, "r");
    interactive = false;
  }
  if (argc > 2) {
    yyout = fopen(argv[2], "w");
  }
  if (interactive) {
    printf(PROMPT);
  }
  yylloc.filename = filename;
  int ret = yyparse();
  if (!interactive) {
    fclose(yyin);
    fclose(yyout);
  }
  return ret;
}

void yyerror(const char *fmt, ...) {
  extern char *yytext;
  fprintf(stderr, "in %s, at %d:%d-%d:%d: ",
      yylloc.filename, yylloc.first_line, yylloc.first_column,
      yylloc.last_line, yylloc.last_column);
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}
