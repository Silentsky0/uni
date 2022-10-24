%{
#include <stdio.h> /* printf() */
#include <string.h> /* strcpy */
#include <stdlib.h> /* atoi(), atof() */
#include "common.h" /* MAX_STR_LEN */
#include "c.tab.h" /* deklaracja symboli końcowych */

char *include_directive = "Processing directive ";

/* wypisanie informacji o znalezionym symbolu końcowym */
int process_token(const char *text, const char *TokenType,
                  const char *TokenVal, const int TokenID);

int comm_beg = 0; /* wiersz rozpoczęcia komentarza */
int str_beg = 0;

%}

/* deklaracja warunków początkowych analizatora */
/* (nie deklarujemy domyślnego warunku INITIAL) */
/* ..................... */

%x TEXT_CONST
%x MULTILINE_COMMENT
%option yylineno


%%
 /* tu należy umieścić odpowiednie reguły w odpowiedniej kolejności */

 /* wykrywanie słów kluczowych zdefiniowanych za pomocą dyrektywy %token */

char {return process_token(yytext, "KW_CHAR", "", KW_CHAR);}
unsigned {return process_token(yytext, "KW_UNSIGNED", "", KW_UNSIGNED);}
short {return process_token(yytext, "KW_SHORT", "", KW_SHORT);}
int {return process_token(yytext, "KW_INT", "", KW_INT);}
long {return process_token(yytext, "KW_LONG", "", KW_LONG);}
float {return process_token(yytext, "KW_FLOAT", "", KW_FLOAT);}
void {return process_token(yytext, "KW_VOID", "", KW_VOID);}
for {return process_token(yytext, "KW_FOR", "", KW_FOR);}
double {return process_token(yytext, "KW_DOUBLE", "", KW_DOUBLE);}
if {return process_token(yytext, "KW_IF", "", KW_IF);}
else {return process_token(yytext, "KW_ELSE", "", KW_ELSE);}
while {return process_token(yytext, "KW_WHILE", "", KW_WHILE);}
do {return process_token(yytext, "KW_DO", "", KW_DO);}
struct {return process_token(yytext, "KW_STRUCT", "", KW_STRUCT);}


 /* usuwanie białych znaków */

[ \n\t]+


 /* usuwanie komentarzy jednowierszowych bez użycia warunków początkowych */

"//".*

 /*wykrywanie operatorów wieloznakowych */

"++" {return process_token(yytext, "INC", "", INC);}
"<=" {return process_token(yytext, "LE", "", LE);}


 /* wykrywanie identyfikatorów */

[a-zA-Z]+[a-zA-Z0-9_]* {return process_token(yytext, "IDENT", yytext, IDENT);}
[a-zA-Z_]+[a-zA-Z0-9]+ {return process_token(yytext, "IDENT", yytext, IDENT);}


 /* wykrywanie liczb bez znaku całkowitych i zmiennoprzecinkowych */

[0-9]+ {return process_token(yytext, "INTEGER_CONST", yytext, INTEGER_CONST);}
[0-9]*"."[0-9]+e?-?[0-9]+ {return process_token(yytext, "FLOAT_CONST", yytext, FLOAT_CONST);}
[0-9]+"."[0-9]*e?-?[0-9]* {return process_token(yytext, "FLOAT_CONST", yytext, FLOAT_CONST);}



 /* wykrywanie stałych tekstowych (napisów) bez użycia mechanizmu warunków początkowych */

 /* \"[^"]*\" {return process_token(yytext, "STRING_CONST", yytext, STRING_CONST);} */


 /* wykrywanie stałych znakowych */

\'(.|\\.)\' {return process_token(yytext, "CHARACTER_CONST", "", CHARACTER_CONST);}


 /* wykrywanie symboli końcowych jednoznakowych: operatorów, interpunkcji */

[\+\-\*/;,=(){}\.\[\]\<] {return process_token(yytext, yytext, "", *yytext);}


 /* wykrywanie dyrektywy dołączania plików */

#include[[:blank:]]+(\"[a-zA-Z0-9]+"."h\")|(<[a-zA-Z0-9]+"."h>) {printf("Processing directive %s\n", yytext);}


 /* wykrywanie napisów z użyciem warunków początkowych */

<INITIAL>\" {BEGIN(TEXT_CONST); yymore(); str_beg = yylineno;}
<TEXT_CONST>\" {BEGIN(INITIAL); return process_token(yytext, "STRING_CONST", yytext, STRING_CONST);}
<TEXT_CONST>. {yymore();}
<TEXT_CONST>\n {BEGIN(INITIAL); printf("Brak konca napisu w wierszu %d\n", str_beg);}

 /* usuwanie komentarzy wielowierszowych z użyciem warunków początkowych */

<INITIAL>"/*" {BEGIN(MULTILINE_COMMENT); comm_beg = yylineno;}
<MULTILINE_COMMENT>"*/" {BEGIN(INITIAL);}
<MULTILINE_COMMENT>.|\n

 /* znajdowanie znaków zamknięcia komentarza przy braku jego rozpoczęcia z
 użyciem warunków początkowych */

<INITIAL>"*/" {printf("Niezidentyfikowane zamknięcie komentarza w wierszu %d\n", yylineno);}

 /* wykrywanie niezamkniętego komentarza ze wskazaniem wiersza jego rozpoczęcia z użyciem warunków początkowych */
 
  /* rozwiązanie w wierszu 180 */

%%

/* Nazwa:       process_token
 * Cel:         Wypisanie informacji o wykrytym elemencie i przekazanie
 *              tej informacji wyżej z ewentualną wartością elementu, jeśli
 *              takowa występuje.
 * Parametry:   text            - (i) tekst, do którego nastąpiło dopasowanie;
 *              TokenType       - (i) napis będący tekstową reprezentacją
 *                                      nazwy typu elementu;
 *              TokenVal        - (i) wartość elementu, o ile występuje;
 *              TokenID         - (i) identyfikator typu elementu zadeklarowany
 *                                      za pomocą dyrektywy %token
 *                                      w pliku c.y lub kod pojedynczego
 *					znaku (w pliku analizatora składniowego
 *					występuje ujęty w apostrofy).
 * Zwraca:      Identyfikator typu elementu (TokenID).
 * Uwagi:       Informacja o wykrytym elemencie zwracana jest
 *              w trzech kolumnach. W pierwszej wypisywany jest tekst,
 *              do którego nastąpiło dopasowanie, w drugiej - typ elementu,
 *              w trzeciej - wartość elementu (o ile występuje).
 */
int process_token(const char *text, const char *TokenType,
                  const char *TokenVal, const int TokenID)
{
  int l;
  printf("%-20.20s%-15s %s\n", text, TokenType, TokenVal);
  switch (TokenID) {

  case INTEGER_CONST:
    yylval.i = atoi(text); break;

  case FLOAT_CONST:
    yylval.d = atof(text); break;

  case IDENT:
    strncpy(yylval.s, text, MAX_STR_LEN); break;

  case STRING_CONST:
    l = strlen(TokenVal);
    yylval.s[0] = '\0';
    strncpy(yylval.s, TokenVal + 1, l - 2 <= MAX_STR_LEN ? l - 1 : MAX_STR_LEN);
    break;

  case CHARACTER_CONST:
    yylval.i = text[1]; break;

  }
  return(TokenID);
}


int yywrap( void )
{ /* funkcja wywoływana po napotkaniu końca strumienia wejściowego

  /* sprawdzenie, czy warunek poczatkowy YY_START różni się od INITIAL. */
  /* Jeżeli tak, to oznacza to niezamknięty komentarz lub stała tekstową
     - wypisujemy informację o błędzie. */
  /* ..................... */

  if (YY_START != INITIAL) {
    if (comm_beg != 0)
      printf("Brak zamknięcia komentarza w wierszu %d\n", comm_beg);
  }

  return( 1 ); /* koniecznie, by analiza nie rozpoczęła się od nowa */
}


