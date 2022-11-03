#ifndef FILE_H
#define FILE_H

#include "record.h"

struct file {
    int num_records;
    struct record *records;
    struct record *overflow;

    void *ov_start;
};

void file_init(struct file* file);
int file_append_record(struct file *file, struct record* record);
int file_import(const char* import_path);

#endif // FILE_H
