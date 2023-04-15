%token NUM
%token IF ELIF ELSE
%token ';' ':'

%%

stmt : IF expr ':' stmt_list opt_elif_list opt_else
     | expr ';'
     ;

opt_elif_list : %empty
              | elif_list
              ;

elif_list : elif_clause
          | elif_clause elif_list
          ;

elif_clause : ELIF ':' stmt_list ;

opt_else : %empty | else_clause ;
else_clause : ELSE ':' stmt_list ;

stmt_list : stmt
          | stmt stmt_list
          ;

expr : NUM
     ;


%%
