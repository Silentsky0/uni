%{
#include <ctype.h>
int l_lit; /* licznik liter w ciągu wejściowym */
%}
ml  [a-z]
%%
{ml}  { printf("%c", toupper(yytext[0])); l_lit++; }
[A-Z] { printf("%c", tolower(yytext[0])); l_lit++; }
%%
int yywrap()
{
  printf("Ciąg wejściowy zawierał %d liter\n", l_lit);
  return 1;
}