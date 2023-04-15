%require "3.8.2"

%language "c++"
%skeleton "lalr1.cc"

%define api.value.type variant
%define api.token.constructor
%define api.token.raw

%define parse.assert
%define parse.trace
%define parse.error detailed
%define parse.lac full

%code requires {
  #include <functional>
  #include <memory>

  #include "list.hpp"
}

%code {
  auto yylex() -> yy::parser::symbol_type;
}

%token <double> NUMBER
%nterm <std::shared_ptr<List>> list

%token
  COMMA    ","
  NEWLINE  "\n"
  ;

%%

prog:
    %empty
  | prog line
  ;

line:
    "\n"
  | list "\n"   { std::cout << "final: " << *($1) << '\n'; }
  ;

list:
   NUMBER           { $$ = std::make_shared<List>(); $$->insert_last($1); std::cout << *($$) << std::endl; }
  | list "," NUMBER { $$ = std::move($1); $$->insert_last($3); std::cout << *($$) << std::endl; }
  ;

%%

auto main() -> int {
  yy::parser parse;
  // parse.set_debug_level(true);
  return parse();
}

auto yy::parser::error(const std::string &msg) -> void {
  std::cerr << msg << '\n';
}

auto yylex() -> yy::parser::symbol_type {
   static int n = 0;
   std::vector<std::function<yy::parser::symbol_type(void)>> data =  {
        []() { return yy::parser::make_NUMBER(42); },
        []() { return yy::parser::make_COMMA(); },
        []() { return yy::parser::make_NUMBER(3.14); },
        []() { return yy::parser::make_COMMA(); },
        []() { return yy::parser::make_NUMBER(2.718); },
        []() { return yy::parser::make_NEWLINE(); },
    };
    struct Defer {
        std::function<void(void)> f;
        Defer(std::function<void(void)> f): f(f) {}
        ~Defer() { f(); }
    };
    if (n >= data.size()) {
        return yy::parser::make_YYEOF();
    }
    Defer d {[]() { n++; }};
    return data[n]();
}
