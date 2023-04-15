%option noyywrap nodefault

%{
  #include <string.h>

  #include "yatl.tab.h"

  void yyerror(const char *fmt, ...);
%}

digit       [0-9]
number      {digit}+(\.{digit}+)([Ee][-+]?{digit}+)
ident       [A-Za-z_][0-9A-Za-z_]*

%%

{number}    {
              strncpy(yylval.sval, yytext, yyleng);
              return NUMBER;
            }

{ident}     { return IDENT; }

[-+*/(),]      { return yytext[0]; }

.           { yyerror("invalid character\n"); }

<<EOF>>     { return YYEOF; }

%%
