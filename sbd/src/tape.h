#ifndef TAPE_H
#define TAPE_H

#include <stdio.h>

struct tape {
    const char *file_path;
    FILE *tape_file;
    int num_runs;
    int num_records;
};

#endif // TAPE_H
