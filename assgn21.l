%{
#include <stdio.h>
%}
%%
[789][0-9]{9}       { printf("Valid Mobile Number: %s\n", yytext); }
[0-9]+              { printf("Invalid Mobile Number: %s\n", yytext); }
.|\n                ;  
%%

int main() {
    yylex();
    return 0;
}
