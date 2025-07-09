%{
#include <stdio.h>
#include <string.h>

// Define C keywords
char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", NULL
};

int is_keyword(char *word) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(keywords[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}
%}

IDENTIFIER    [a-zA-Z_][a-zA-Z0-9_]*
NUMBER        [0-9]+
OPERATOR      (\+|\-|\*|\/|\=|\<|\>|\+\+|\-\-|\<=|\>=|==|!=)
SEPARATOR     [\{\}\[\]\(\)\;\,]

%%
{IDENTIFIER}  {
                if (is_keyword(yytext))
                    printf("Keyword: %s\n", yytext);
                else
                    printf("Identifier: %s\n", yytext);
              }

{NUMBER}      { printf("Number: %s\n", yytext); }

{OPERATOR}    { printf("Operator: %s\n", yytext); }

{SEPARATOR}   { printf("Separator: %s\n", yytext); }

[ \t\n]       { /* Ignore whitespace */ }

.             { printf("Unrecognized token: %s\n", yytext); }
%%

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("File opening failed");
        return 1;
    }

    yyin = file;
    yylex();
    fclose(file);
    return 0;
}
