#include "disk.h"
#include "common/status.h"
#include "record.h"
#include <stdlib.h>
#include <time.h>
#include <memory.h>


int file_size(FILE **file);


int disk_open_file(struct tape *tape, const char *mode) {

    tape->file = fopen(tape->path, mode);
    if (tape->file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, tape->path);
        return -EIO;
    }

    tape->num_records = 0;
    tape->num_runs = 0;
    tape->fib = 0;

    buffer_init(&tape->buffer);

    return 0;
}

void disk_close_file(struct tape *tape) {

    // if tape is not meant to be written to
    if (tape->file_mode[0] == 'r') {
        fclose(tape->file);
        return;
    }

    struct buffer *buffer = &tape->buffer;

    struct record incorrect_record;
    generate_incorrect_record(&incorrect_record);

    // write partially full block if it contains some leftover records
    if (buffer->buffer_records != RECORDS_IN_BLOCK) {

        for (int i = buffer->buffer_records; i < RECORDS_IN_BLOCK; i++) {
            memcpy(&buffer->block->records[buffer->record_index], &incorrect_record, sizeof(struct record));
            buffer->record_index += 1;
        }

        write_block(&tape->file, buffer->block_index, buffer->block);
    }

    buffer_clear(buffer);

    printf("Closing file %s, its contents:\n", tape->path);
    disk_print_file(tape);

    buffer_close(buffer);

    fclose(tape->file);
}

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

        write_block(&file, blocks_to_generate, &leftover_block);
    }

    fclose(file);

    return 0;
}

/*
 * Get next record from file
 *
 * Record that was read is placed in *record
 * 
 * Returns ENOFILE (-4) when end of file is reached, then record read is marked
 * as incorrect
 */
int disk_get_next_record(struct tape *tape, struct record *record) {
    // TODO get next should possibly skip empty records 
    struct buffer *buffer = &tape->buffer;

    if (buffer->block_index == file_size(&tape->file) / BLOCK_SIZE) {
        printf("%s: end of file reached\n", __func__); // TODO remove this message
        generate_incorrect_record(record);
        return -ENOFILE;
    }

    if (buffer->record_index == 0) {
        int status = read_block(&tape->file, buffer->block_index, buffer->block);
        if (status < 0) {
            printf("%s: some error reading block\n", __func__);
            return status;
        }
    }

    // TODO append to buffer could be a function
    memcpy(record, &buffer->block->records[buffer->record_index], sizeof(struct record));
    buffer->buffer_records += 1;

    buffer->record_index += 1;
    if (buffer->record_index == RECORDS_IN_BLOCK) {
        buffer->block_index += 1;
        buffer->record_index = 0;
    }

    return 1;
}


/*
 * Append record to file
 *
 * Record is placed 
 */
int disk_append_record(struct tape *tape, struct record *record) {
    
    struct buffer *buffer = &tape->buffer;

    memcpy(&buffer->block->records[buffer->record_index], record, sizeof(struct record));
    buffer->buffer_records += 1;

    buffer->record_index += 1;
    if (buffer->record_index == RECORDS_IN_BLOCK) {

        int status = write_block(&tape->file, buffer->block_index, buffer->block);
        if (status < 0) {
            printf("%s: some error writing block\n", __func__);
            return status;
        }
        
        buffer_clear(buffer);

        buffer->record_index = 0;
        buffer->block_index += 1;
    }

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

    // padding
    for (int byte = 0; byte < BLOCK_SIZE - RECORDS_IN_BLOCK * sizeof(struct record); byte++) {
        block->padding[byte] = -1;
    }

    status = fwrite((void *) block, BLOCK_SIZE, 1, *file);
    if (status < 1) {
        perror("Error: ");
        printf("%s: fwrite error %d\n", __func__, ferror(*file));
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
        printf("%s: error reading block\n", __func__);
    }

    printf(" Block %d\n", index);
    for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
        printf(" ");
        record_print(&block_to_read.records[i], RECORD_PRINT_ID | RECORD_PRINT_NAME | RECORD_PRINT_EMPTY_RECORDS);
    }
}

void disk_print_file(struct tape *tape) {
    printf("%s:     %d runs last record id %s\n", tape->path, tape->num_runs, tape->last_record.id.identity_series);
    for (int i = 0; i < file_size(&tape->file) / BLOCK_SIZE; i++) {
        print_block(&tape->file, i);
    }

    printf(" Buffer:\n");
    for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
        record_print(&tape->buffer.block->records[i], RECORD_PRINT_ID | RECORD_PRINT_NAME);
    }
}
