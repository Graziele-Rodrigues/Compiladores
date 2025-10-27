%option noyywrap

%option c++
%{
#include <iostream>
using namespace std;
%}

%%

[a-zA-Z]+   { cout << "Palavra: " << yytext << endl; }
[0-9]+      { cout << "Número: " << yytext << endl; }
.|\n        ;   // ignora outros caracteres

%%
