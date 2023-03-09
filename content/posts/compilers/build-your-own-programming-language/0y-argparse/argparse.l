%{
  #include <stdlib.h>
  #include <string.h>
  #include <stdint.h>
  #include <stdio.h>

  #include "argparse.tab.h"

  void yyerror(const char *fmt, ...);
%}

%option noyywrap nodefault

short_name -[0-9A-Za-z?]
long_name  --[0-9A-Za-z]+(-[0-9A-Za-z])*

string     .+
number     [0-9]+

%%

{number}    {
              yylval.ival = atoi(yytext);
              return NUMBER;
            }


{short_name}    {
                  strncpy(yylval.sval, yytext + 1, 1024);
                  return SHORT_NAME;
                }

{long_name}     {
                  strncpy(yylval.sval, yytext + 2, 1024);
                  return LONG_NAME;
                }

{string}    {
              strncpy(yylval.sval, yytext, 1024);
              return STRING;
            }

[ \t\r\n\f] ;

.           { yyerror("invalid character `%c'\n", yytext[0]); }

<<EOF>>     { return YYEOF; }

%%
