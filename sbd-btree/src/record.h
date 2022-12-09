#ifndef RECORD_H
#define RECORD_H

#define NAME_MAX_LENGTH 32

#define RECORD_PRINT_ID              0x1
#define RECORD_PRINT_NAME            0x2
#define RECORD_PRINT_EMPTY_RECORDS   0x4

struct id {
    char identity_series[4];
    int identity_number;
} __attribute__((__packed__));

struct data {
    char name[NAME_MAX_LENGTH];
    char surname[NAME_MAX_LENGTH];
    unsigned short age;
} __attribute__((__packed__));

struct record {
    long id;
    struct id identity;
    struct data data;
} __attribute__((__packed__));

struct record tmp_record();
struct record tmp_record_by_key(long id);

#endif // RECORD_H
