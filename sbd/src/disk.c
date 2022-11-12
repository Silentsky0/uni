#include "disk.h"
#include "common/status.h"
#include "record.h"
#include <stdlib.h>

#include <time.h>

int disk_generate_random(const char *path, int number_of_records)
{
    unique_random_numbers(number_of_records);

    FILE *file = fopen(path, "wb");
    if (file == NULL)
        return -EIO;

    int blocks_to_generate = number_of_records / RECORDS_IN_BLOCK;

    struct block new_block;
    struct record new_record;

    for (int i = 0; i < blocks_to_generate; i++) {
        for (int j = 0; j < RECORDS_IN_BLOCK; j++) {
            generate_random_record(&new_record);
            new_block.records[j] = new_record;
        }

        // padding
        for (int byte = 0; byte < BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record); byte++) {
            new_block.padding[byte] = -1;
        }

        write_block(&file, i, &new_block);
    }

    // some number of records that do not fill the whole block
    int leftover_records = number_of_records - blocks_to_generate * RECORDS_IN_BLOCK;

    if (leftover_records > 0) {
        struct block leftover_block;

        for (int i = 0; i < leftover_records; i++) {
            generate_random_record(&new_record);
            leftover_block.records[i] = new_record;
        }
        // fill the block with incorrect records
        for (int i = leftover_records; i < RECORDS_IN_BLOCK; i++) {
            generate_incorrect_record(&new_record);
            leftover_block.records[i] = new_record;
        }
        // padding
        for (int byte = 0; byte < BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record); byte++) {
            leftover_block.padding[byte] = -1;
        }
        write_block(&file, blocks_to_generate, &leftover_block);
    }

    fclose(file);

    return 0;
}

int file_size(FILE **file) {
    fseek(*file, 0L, SEEK_END);
    int size = (int) ftell(*file);
    rewind(*file);
    return size;
} 

int write_block(FILE **file, int index, struct block *block) {
    int status = fseek(*file, index * BLOCK_SIZE, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    status = fwrite((void *) block, BLOCK_SIZE, 1, *file);
    if (status < 1) {
        printf("%s: fwrite error\n", __func__);
        return -EIO;
    }

    
    return 0;
}

int read_block(FILE **file, int index, struct block *block) {

    int status = fseek(*file, index * BLOCK_SIZE, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    status = fread((void *) block, BLOCK_SIZE, 1, *file);
    if (status < 1) {
        printf("%s: fread error\n", __func__);
        return -EIO;
    }

    return 0;
}

void print_block(FILE **file, int index) {

    struct block block_to_read;

    int status = read_block(file, index, &block_to_read);
    if (status < 0) {
        printf("%s: error reading block", __func__);
    }

    printf(" Block %d\n", index);
    for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
        printf(" ");
        record_print(&block_to_read.records[i], RECORD_PRINT_ID);
    }

}

void disk_print_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, path);
        return;
    }

    printf("%s:\n", path);
    for (int i = 0; i < file_size(&file) / BLOCK_SIZE; i++) {
        print_block(&file, i);
    }
}
