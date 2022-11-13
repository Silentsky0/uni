#ifndef TAPE_H
#define TAPE_H

#include <stdio.h>
#include "buffer.h"
#include "record.h"

struct tape {
    const char *path;
    FILE *file;

    struct record last_record;

    int num_runs;
    int num_records;
    int fib;

    struct buffer buffer;
};

#endif // TAPE_H
