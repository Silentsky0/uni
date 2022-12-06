#include "record.h"
#include "common/config.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "common/status.h"

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
    const int max_random_value = 99999 - 1;
    to_generate = 99999 - 1;

    srand(time(NULL));
    
    // TODO index < max_random_value was here
    for (index = 0; generated_ids_number < to_generate; index++) {
        int rn = max_random_value - index;
        int rm = to_generate - generated_ids_number;
        if (rand() % rn < rm)
            generated_ids[generated_ids_number++] = index + 1;
    }

    assert(generated_ids_number == to_generate);

    for (int i = 0; i < to_generate; i++) {
        int j = i + rand() % (to_generate - i);
        int tmp = generated_ids[i];
        generated_ids[i] = generated_ids[j];
        generated_ids[j] = tmp;
    }
}

int record_is_equal(struct record* r1, struct record* r2) {
    int diff = 0;
    for (int k = 0; k < 4; k++) {
        diff += abs((int) r1->id.identity_series[k] - (int) r2->id.identity_series[k]);
    }

    if (r1->id.identity_number == r2->id.identity_number && diff == 0) {
        //printf("%s: this record already exists!!\n", __func__);
        return 1;
    }

    return 0;
}

/*
 * 0 if any of the records is empty
 * positive value if r1 is greater
 * negative value if r2 is greater
 * value indicates difference between the two
 */
int record_compare(struct record* r1, struct record* r2) {

    if (record_is_empty(r1) || record_is_empty(r2))
        return 0;

    if (record_is_equal(r1, r2))
        return -10000;

    int diff = 0;

    for (int i = 0; i < 3; i++) {
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

    if (generated_id_index == 100000)
        generated_id_index = 0;
}

int hash(const char* str, int h)
{
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

void generate_next_record(struct record *r) {
    static int number_index = 1;
    static int char_index[3] = {0, 0, 0};

    for (int i = 0; i < 3; i++) {
        r->id.identity_series[i] = (char)('A' + char_index[i]);
    }
    char_index[2] += 1;
    if (char_index[2] == 25) {
            char_index[2] = 0;
            char_index[1] += 1;
        }
    if (char_index[1] == 25) {
        char_index[1] = 0;
        char_index[0] += 1;
    }
    if (char_index[0] == 25) {
        char_index[0] = 0;
        //number_index += 1;
    }
    r->id.identity_number = generated_ids[number_index];
    number_index += 1;
    if (number_index >= generated_ids_number)
        number_index = 0;

    // TODO hashing might be wrong
    // int identity_hash = hash(r->id.identity_series, 73); // some prime magic number
    // for (int i = 0; i < 3; i++) {
    //     char letter;
    //     letter = (char) 'A' + (identity_hash % 25);
    //     identity_hash /= 25;

    //     r->id.identity_series[i] = letter;
    // }

    // truly random stuff
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

