    #include <stdlib.h>

    #define nitems(a) ((sizeof(a)) / (sizeof(a[0])))
    int result = 0;

%%

[0-9]* {printf("%s + ", yytext); result += atoi(yytext);}

[^0-9]

[0-9]*$ {result += atoi(yytext); printf("%s = %d\n", yytext, result); result = 0; }

%%

