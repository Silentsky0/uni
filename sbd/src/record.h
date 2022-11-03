#ifndef RECORD_H
#define RECORD_H

struct id {
    char identity_series[4];
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
void record_print(struct record r);

#endif // RECORD_H
