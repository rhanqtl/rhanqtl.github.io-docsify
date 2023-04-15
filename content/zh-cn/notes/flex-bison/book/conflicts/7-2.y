%token A R S

%%

s  : x1
   | x2
   | x3
   | y R
   ;
x1 : A R  ;
x2 : A z1 ;
x3 : A z2 ;
y  : A ;
z1 : R ;
z2 : S ;

%%
