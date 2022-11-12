#ifndef DISK_H
#define DISK_H

#include "common/config.h"
#include "record.h"
#include <stdio.h>

// block struct contains records that fit into that block and some padding
struct block {
    struct record records[RECORDS_IN_BLOCK];
    char padding[BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record)];
} __attribute__((__packed__));

int disk_open_file(FILE **file, const char *path);
void disk_close_file(FILE **file);
int disk_get_next_record(FILE **file, struct record *record);

int write_block(FILE **file, int index, struct block *block);
int read_block(FILE **file, int index, struct block *block);

int disk_generate_random(const char *path, int number_of_records);
void disk_print_file(const char *path);

#endif
