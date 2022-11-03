#ifndef RECORD_H
#define RECORD_H

struct id {
    const char *identity_series;
    int identity_number;
} __attribute__((__packed__));

struct data {
    const char* name;
    const char* surname;
    int age;
} __attribute__((__packed__));

struct record {
    struct id id;
    struct data data;
} __attribute__((__packed__));

int record_compare(struct record* r1, struct record* r2);
const char *record_to_string(struct record* r);

#endif // RECORD_H
