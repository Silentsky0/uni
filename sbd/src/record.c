#include "record.h"
#include <string.h>
#include <stdio.h>

int record_compare(struct record* r1, struct record* r2) {
    return r1->id.identity_number > r2->id.identity_number;
}

const char * record_to_string(struct record *r) {
    return strcat("record ", r->data.name);
}

void record_print(struct record r) {
    printf("--- Record ---\n");
    printf("    id = %s%d\n", r.id.identity_series, r.id.identity_number);
    printf("    name = %s %s\n\n", r.data.name, r.data.surname);
}
