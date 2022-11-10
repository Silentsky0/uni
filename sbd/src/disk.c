#include "disk.h"
#include "common/status.h"
#include "record.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int write_block(FILE **file, int index, struct block *block);

int disk_generate_random(const char *path, int number_of_records)
{
    srand(time(NULL));
    unique_random_numbers(number_of_records);

    FILE *file = fopen(path, "wb");
    if (file == NULL)
        return -EIO;

    int blocks_to_generate = number_of_records / RECORDS_IN_BLOCK;

    struct block new_block;
    struct record new_record;

    for (int i = 0; i < blocks_to_generate; i++) {
        for (int j = 0; j < RECORDS_IN_BLOCK; j++) {
            random_record(&new_record);
            new_block.records[j] = new_record;
        }

        // padding
        for (int byte = 0; byte < BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record); byte++) {
            new_block.padding[byte] = -1;
        }

        write_block(&file, i, &new_block);
    }

    // block with some number of records that do not fill the whole block
    struct block leftover_block;
    int leftover_records = number_of_records - blocks_to_generate * RECORDS_IN_BLOCK;

    for (int i = 0; i < leftover_records; i++) {
        random_record(&new_record);
        leftover_block.records[i] = new_record;
    }
    // fill the block with incorrect records
    for (int i = leftover_records; i < RECORDS_IN_BLOCK; i++) {
        incorrect_record(&new_record);
        leftover_block.records[i] = new_record;
    }
    // padding
    for (int byte = 0; byte < BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record); byte++) {
        leftover_block.padding[byte] = -1;
    }
    write_block(&file, blocks_to_generate, &leftover_block);

    fclose(file);

    return 0;

}

int write_block(FILE **file, int index, struct block *block) {
    fseek(*file, index * BLOCK_SIZE, 0);

    int status = fwrite((void *) block, BLOCK_SIZE, 1, *file);

    if (status < 1)
        return -EIO;
    
    return 0;
}
