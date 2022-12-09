#include "record.h"

struct record tmp_record() {
    struct record record;
    
    record.id = 37;
    record.identity.identity_number = 23254;
    const char *identity = "abc";
    for (int i = 0; i < 3; i++) {
        record.identity.identity_series[i] = identity[i];
    }

    return record;
}
