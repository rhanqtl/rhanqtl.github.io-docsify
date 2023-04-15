%token I Z

%%

start      : outer Z ;
outer      : %empty
           | outer outer_item ;
outer_item : inner ;
inner      : %empty
           | inner inner_item ;
inner_item : I ;

%%
