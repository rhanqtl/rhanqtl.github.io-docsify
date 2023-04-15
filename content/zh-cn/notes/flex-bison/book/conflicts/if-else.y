%token NUM
%token IF THEN ELSE END
%token ';'

%%

stmt : IF expr THEN stmt_list END
     | IF expr THEN stmt_list ELSE stmt END
     | expr ';'
     ;

stmt_list : stmt
          | stmt stmt_list
          ;

expr : NUM
     ;


%%
