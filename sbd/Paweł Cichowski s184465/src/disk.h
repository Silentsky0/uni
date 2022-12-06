#ifndef DISK_H
#define DISK_H

#include "common/config.h"
#include "record.h"
#include <stdio.h>
#include "tape.h"

// block struct contains records that fit into that block and some padding
struct block {
    struct record records[RECORDS_IN_BLOCK];
    char padding[BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record)];
} __attribute__((__packed__));

int file_size(FILE **file);
int get_write_operations_number();
int get_read_operations_number();
void set_read_operations_number(int val);
void set_write_operations_number(int val);

int disk_open_file(struct tape *tape, const char *mode);
void disk_close_file(struct tape *tape);

int disk_get_next_record(struct tape *tape, struct record *record);
int disk_append_record(struct tape *tape, struct record *record);

int write_block(FILE **file, int index, struct block *block);
int read_block(FILE **file, int index, struct block *block);

int disk_generate_random(const char *path, int number_of_records);
int disk_generate_records(const char *path, int to_generate);
void disk_print_file(struct tape *tape);
void disk_debug_tape(struct tape *tape);
int disk_keyboard_input(const char *path);

#endif
