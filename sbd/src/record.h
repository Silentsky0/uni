#ifndef RECORD_H
#define RECORD_H

#define NAME_MAX_LENGTH 32

#define RECORD_PRINT_ID              0x1
#define RECORD_PRINT_NAME            0x2
#define RECORD_PRINT_EMPTY_RECORDS   0x4

#define RECORD_FLAG_DEFAULT          0x0
#define RECORD_FLAG_DELETED          0x1
#define RECORD_FLAG_INCORRECT        0x2
#define RECORD_FLAG_LAST_OF_A_SERIES 0x4

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
    struct id id;
    struct data data;
} __attribute__((__packed__));

int record_compare(struct record* r1, struct record* r2);
int record_is_empty(struct record *r);
const char *record_to_string(struct record* r);
void record_print(struct record *r, char flags);

void generate_random_record(struct record *r);
void generate_incorrect_record(struct record *r);
void unique_random_numbers(int to_generate);

#endif // RECORD_H
