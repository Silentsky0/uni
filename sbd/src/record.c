#include "record.h"
#include "common/config.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#define RANDOM_NAMES_NUM 20
#define RANDOM_MALE_SURNAMES_NUM 10
#define RANDOM_FEMALE_SURNAMES_NUM 10

int generated_ids[MAX_RANDOM_IDS];
int generated_ids_number = 0;

char random_letter() {
    return (char) 'A' + (rand() % 26);
}

const char *random_names[RANDOM_NAMES_NUM] = {"Maurycy","Alfred","Hubert","Konrad","Krzysztof","Paweł","Krystian","Fryderyk","Heronim","Adam","Maria","Beata","Agnieszka","Katarzyna","Jagoda","Martyna","Aleksandra","Julia","Klara","Weronika",
};
const char *random_male_surnames[RANDOM_MALE_SURNAMES_NUM] = {"Maciejewski","Szulc","Zieliński","Jaworski","Piotrowski","Czerwiński","Cieślak","Szymański","Kaczmarczyk","Kamiński",
};
const char *random_female_surnames[RANDOM_FEMALE_SURNAMES_NUM] = {"Baran","Kowalska","Woźniak","Piotrowska","Zalewska","Zawadzka","Lewandowska","Witkowska","Urbańska","Szulc",
};

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

/*
 * 0 if records are identical
 * positive value if r1 is greater
 * negative value if r2 is greater
 * value indicates difference between the two
 */
int record_compare(struct record* r1, struct record* r2) {

    if (record_is_empty(r2))
        return 0;

    int diff;

    for (int i = 0; i < 4; i++) {
        diff = (int) r1->id.identity_series[i] - (int) r2->id.identity_series[i];
        
        if (diff != 0)
            return diff;
    }

    return r1->id.identity_number - r2->id.identity_number;
}

const char * record_to_string(struct record *r) {
    return strcat("record ", r->data.name);
}

int record_is_empty(struct record *r) {
    return r->id.identity_number <= 0;
}

void record_print(struct record *r, char flags) {
    if (record_is_empty(r) && flags & RECORD_PRINT_EMPTY_RECORDS) {
        printf ("  empty record\n");
        return;
    }

    if (record_is_empty(r))
        return;

    if (!flags & RECORD_PRINT_ID)
        goto name;

    if (r->id.identity_number < 10) {
        printf("  id = %s0000%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    if (r->id.identity_number < 100) {
        printf("  id = %s000%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    if (r->id.identity_number < 1000) {
        printf("  id = %s00%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    if (r->id.identity_number < 10000) {
        printf("  id = %s0%d", r->id.identity_series, r->id.identity_number);
        goto name;
    }
    printf("  id = %s%d", r->id.identity_series, r->id.identity_number);

name:

    if (flags & RECORD_PRINT_NAME) {
        printf("  name = %s %s\n", r->data.name, r->data.surname);
    }
    else {
        printf("\n");
    }
}

void generate_random_record(struct record *r) {

    static int generated_id_index = 0;

    r->id.identity_number = generated_ids[generated_id_index]; // random number with 5 digits

    strcpy(r->id.identity_series, "AAA");
    for (int i = 0; i < 3; i++) {
        r->id.identity_series[i] = random_letter();
    }

    int random_name_index = rand() % RANDOM_NAMES_NUM;
    strcpy(r->data.name, random_names[random_name_index]);

    // random name is male
    if (random_name_index < RANDOM_NAMES_NUM - RANDOM_FEMALE_SURNAMES_NUM) {
        int random_surname_index = rand() % RANDOM_MALE_SURNAMES_NUM;

        strcpy(r->data.surname, random_male_surnames[random_surname_index]);
    }
    else {
        int random_surname_index = rand() % RANDOM_FEMALE_SURNAMES_NUM;

        strcpy(r->data.surname, random_female_surnames[random_surname_index]);
    }

    r->data.age = rand() % 100;

    generated_id_index += 1;
}

void generate_incorrect_record(struct record *r) {
    strcpy(r->id.identity_series, "---");
    r->id.identity_number = -1;

    for (int i = 0; i < NAME_MAX_LENGTH; i++) {
        r->data.name[i] = '0';
        r->data.surname[i] = '0';
    }

    r->data.age = -1;
}

