#include "record.h"
#include "string.h"

int record_compare(struct record* r1, struct record* r2) {
    return r1->id.identity_number > r2->id.identity_number;
}

const char * record_to_string(struct record* r) {
    return strcat("record ", r->data.name);
}
