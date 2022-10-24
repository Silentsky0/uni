
    int **counters = NULL;

    #define keywords {"for", "if"}

    #define nitems(a) ((sizeof(a)) / (sizeof(a[0])))

%%

(int|char|void|for|if|else) {}

[a-zA-Z][a-zA-Z0-9]* {printf("123");}


[a-zA-Z0-9]* {}

%%

void add_counter() {

}
