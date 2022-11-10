#include "record.h"
#include "common/config.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

int generated_ids[MAX_RANDOM_IDS];
int generated_ids_number = 0;

char random_letter() {
    return (char) 'A' + (rand() % 26);
}

/*
 * Unique random numbers with 5 digits
 * Knuth algorithm
 */
void unique_random_numbers(int to_generate) {
    int index;
    const int max_random_value = 99999;

    srand(time(NULL));

    for (index = 0; index < max_random_value && generated_ids_number < to_generate; index++) {
        int rn = max_random_value - index;
        int rm = to_generate - generated_ids_number;
        if (rand() % rn < rm)
            generated_ids[generated_ids_number++] = index;
    }

    assert(generated_ids_number == to_generate);

    for (int i = 0; i < to_generate; i++) {
        int j = i + rand() % (to_generate - i);
        int tmp = generated_ids[i];
        generated_ids[i] = generated_ids[j];
        generated_ids[j] = tmp;
    }
}

int record_compare(struct record* r1, struct record* r2) {
    int result;

    if ((result = strcmp(r1->id.identity_series, r1->id.identity_series)) != 0) {
        return result;
    }

    return r1->id.identity_number > r2->id.identity_number;
}

const char * record_to_string(struct record *r) {
    return strcat("record ", r->data.name);
}

void record_print(struct record *r) {
    printf("--- Record ---\n");
    if (r->id.identity_number < 10) {
        printf("    id = %s0000%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    if (r->id.identity_number < 100) {
        printf("    id = %s000%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    if (r->id.identity_number < 1000) {
        printf("    id = %s00%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    if (r->id.identity_number < 10000) {
        printf("    id = %s0%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    printf("    id = %s%d", r->id.identity_series, r->id.identity_number);

name:
    printf("    name = %s %s\n\n", r->data.name, r->data.surname);
}

void generate_random_record(struct record *r) {

    static int generated_id_index = 0;

    r->id.identity_number = generated_ids[generated_id_index]; // random number with 5 digits

    strcpy(r->id.identity_series, "AAA");
    for (int i = 0; i < 3; i++) {
        r->id.identity_series[i] = random_letter();
    }

    strcpy(r->data.name, "random_name"); // TODO random names
    strcpy(r->data.surname, "random_surname"); // TODO random surnames

    r->data.age = rand() % 100;

    generated_id_index += 1;
}

void generate_incorrect_record(struct record *r) {
    strcpy(r->id.identity_series, "---");
    r->id.identity_number = -1;

    for (int i = 0; i < NAME_CHAR_LENGTH; i++) {
        r->data.name[i] = '0';
        r->data.surname[i] = '0';
    }

    r->data.age = -1;
}

