%{
#include	<stdio.h>
#include	"common.h" /* MAX_STR_LEN */
  /* prototypy */
union YYSTYPE;
int yylex(void);
int yyerror(const char *txt);
%}


%union
{
  char s[MAX_STR_LEN + 1]; /* pole tekstowe dla nazw itp. */
  int i; /* pole całkowite */
  double d; /* pole zmiennoprzecinkowe */
}

%token<i> KW_CHAR KW_UNSIGNED KW_SHORT KW_INT KW_LONG KW_FLOAT KW_VOID KW_FOR
%token<i> KW_DOUBLE KW_IF KW_ELSE KW_WHILE KW_DO KW_STRUCT
%token<i> INTEGER_CONST FLOAT_CONST IDENT STRING_CONST CHARACTER_CONST
%token<i> INC LE

%%


Grammar: /* empty */
	| TOKENS
;

TOKENS: TOKEN
	| TOKENS TOKEN
;

TOKEN: KW_CHAR | KW_UNSIGNED | KW_INT | KW_DOUBLE | KW_VOID | KW_FOR
	| INTEGER_CONST | FLOAT_CONST | IDENT | STRING_CONST | CHARACTER_CONST
	| KW_LONG | KW_STRUCT
	| INC | LE | '+' | '-' | '*' | '/' | ';' | ',' | '='
	| '(' | ')' | '{' | '}' | '.' | '[' | ']' | '<'
	| error
;

%%

/***************************************************************************/
/*                            programs section                             */
/***************************************************************************/

int main( void )
{
	int ret;
	printf( "Autor: Paweł Cichowski\n" );
	printf( "yytext              Typ symbolu      Wartość symbolu znakowo\n\n" );
	ret = yyparse();
	return ret;
}

int yyerror( const char *txt )
{
	printf( "Syntax error %s\n", txt );
}
