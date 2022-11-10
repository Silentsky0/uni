#ifndef RECORD_H
#define RECORD_H

#include "common/config.h"

#define NAME_CHAR_LENGTH 32

struct id {
    char identity_series[4];
    int identity_number;
} __attribute__((__packed__));

struct data {
    char name[NAME_CHAR_LENGTH];
    char surname[NAME_CHAR_LENGTH];
    unsigned short age;
} __attribute__((__packed__));

struct record {
    struct id id;
    struct data data;
} __attribute__((__packed__));

int record_compare(struct record* r1, struct record* r2);
const char *record_to_string(struct record* r);
void record_print(struct record *r);
void generate_random_record(struct record *r);
void generate_incorrect_record(struct record *r);
void unique_random_numbers(int to_generate);

#endif // RECORD_H
