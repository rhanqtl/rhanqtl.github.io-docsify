%option noyywrap nodefault

%{
  #include "tie_in.tab.h"

  extern int unmatched_open_bracket;

  void yyerror(const char *msg);
%}

ident   [A-Za-z][A-Za-z0-9]+

%%

{ident}     {
              yylval.sval = strdup(yytext);
              return IDENT;
            }

"<"     { return LT; }
">"     { return GT; }
">>"    {
          if (unmatched_open_bracket == 0) {
            return RSHIFT;
          }
          unput('>');
          return GT;
        }

\n          { return yytext[0]; }
[ \t\r\f]   ;


.   { yyerror("invalid char\n"); }

<<EOF>>     { return YYEOF; }

%%
