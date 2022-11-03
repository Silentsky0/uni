#ifndef FILE_H
#define FILE_H

#include "record.h"

struct file {
    int num_records;
    struct record *records;
};

int file_init(void);
int file_append_record(struct record* record);


#endif // FILE_H
