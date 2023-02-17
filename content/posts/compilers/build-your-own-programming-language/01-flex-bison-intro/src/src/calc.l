%{
  #include <stdlib.h>

  #include "location.h"
  #include "calc.tab.h"

  extern YYLTYPE yylloc;

  int yycolumn = 1;

  #define YY_USER_ACTION \
    do { \
      yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn; \
      yylloc.last_column = yycolumn + yyleng - 1; \
      yycolumn += yyleng;   \
    } while (0);

  void yyerror(const char *fmt, ...);
%}

%option noyywrap

number  [1-9][0-9]*(\.[0-9]+)?([eE][-+]?[0-9]+)?
number_0 0+[0-9]+
ident   [A-Za-z][0-9A-Za-z_]*

%%

{number}    {
              yylval = atof(yytext);
              return NUM;
            }
{number_0}  {
              yyerror("leading 0's is not supported yet\n");
            }

[-+*/^()]   { return yytext[0]; }

{ident}     {
              yyerror("identifier `%s' found, the feature is not yet supported\n", yytext);
            }

\n          {
              yycolumn = 1;
              return yytext[0];
            }
[ \t\r\f]   ;

"#".*       ;
.           yyerror("invalid character `%c'\n", yytext[0]);

%%
