#include "disk.h"
#include "common/status.h"
#include "record.h"
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <stdio.h>

long disk_num_write_operations = 0;
long disk_num_read_operations = 0;

int get_write_operations_number() {
    return disk_num_write_operations;
}
int get_read_operations_number() {
    return disk_num_read_operations;
}
void set_write_operations_number(int val) {
    disk_num_write_operations = val;
}
void set_read_operations_number(int val) {
    disk_num_read_operations = val;
}

int disk_open_file(struct tape *tape, const char *mode) {

    tape->file_mode = mode;
    tape->file = fopen(tape->path, mode);
    if (tape->file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, tape->path);
        return -EIO;
    }

    buffer_init(&tape->buffer);

    return 0;
}

void disk_close_file(struct tape *tape) {

    //printf ("%s: file size was %d MB\n\n", tape->path, file_size(&tape->file) / 1024 / 1024);

    // if tape is not meant to be written to
    if (tape->file_mode[0] == 'r') {
        // printf("Closing file %s without a buffer, its contents:\n", tape->path);
        // disk_print_file(tape);
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

    // printf("Closing file %s, its contents:\n", tape->path);
    // disk_print_file(tape);

    buffer_close(buffer);

    fclose(tape->file);
}

/*
 * Checks entire disk if record exists
 * requires file to be opened
 */
int disk_record_exists(FILE **file, struct block *unwritten_block, struct record *r, int blocks_in_file) {

    static int times_checked = 0;

    for (int i = 0; i < blocks_in_file; i++) {
        struct block block_to_read;
        int status = read_block(file, i, &block_to_read);
        if (status < 0) {
            printf("%s: error reading block\n", __func__);
        }
        for (int j = 0; j < RECORDS_IN_BLOCK; j++) {
            struct record iterator = block_to_read.records[j];

            int diff = 0;
            for (int k = 0; k < 4; k++) {
                diff += abs((int) r->id.identity_series[k] - (int) iterator.id.identity_series[k]);
            }

            if (iterator.id.identity_number == r->id.identity_number && diff == 0) {
                printf("%s: this record already exists!!\n", __func__);
                return 1;
            }
        }
    }

    for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
        struct record iterator = unwritten_block->records[i];

        if (record_is_empty(&iterator))
            continue;

        int diff = 0;
        for (int j = 0; j < 4; j++) {
           diff += abs((int) r->id.identity_series[j] - (int) iterator.id.identity_series[j]);
        }

        if (iterator.id.identity_number == r->id.identity_number && diff == 0) {
            printf("%s: this record already exists!!\n", __func__);
            return 1;
        }
    }

    if (times_checked >= 100000) {
        printf("times checked: %d", times_checked);
    }

    return 0;
}

int disk_generate_random(const char *path, int number_of_records)
{
    unique_random_numbers(number_of_records);

    FILE *file = fopen(path, "wb+");
    if (file == NULL)
        return -EIO;

    int blocks_to_generate = number_of_records / RECORDS_IN_BLOCK;

    struct block new_block;
    struct record new_record;

    for (int i = 0; i < blocks_to_generate; i++) {
        for (int j = 0; j < RECORDS_IN_BLOCK; j++) {

            generate_random_record(&new_record);
            //while (disk_record_exists(&file, &new_block, &new_record, i)) {
            //    generate_random_record(&new_record);
            //}
            new_block.records[j] = new_record;
        }

        write_block(&file, i, &new_block);
    }

    // // some number of records that do not fill the whole block
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

int disk_generate_records(const char *path, int to_generate) {

    printf("\n=== Data generation phase ===\n\n");

    if (to_generate < 50000) {
        return disk_generate_random(path, to_generate);
    }

    unique_random_numbers(to_generate);

    FILE *file = fopen(path, "wb+");
    if (file == NULL)
        return -EIO;

    struct record r;
    struct block b;
    int blocks_to_generate = to_generate / RECORDS_IN_BLOCK + 1;

    for (int i = 0; i < blocks_to_generate; i++) {
        for (int j = 0; j < RECORDS_IN_BLOCK; j++) {
            generate_next_record(&r);
            b.records[j] = r;
        }

        write_block(&file, i, &b);
    }

    // shuffle blocks
    // for (int i = 0; i < blocks_to_generate; i++) {
    //     int j = i + rand() % (blocks_to_generate - i);

    //     struct block b1;
    //     struct block b2;

    //     int status = read_block(&file, i, &b1);
    //     status += read_block(&file, j, &b2);


    //     // TODO shuffle records inside block
    //     for (int p = 0; p < RECORDS_IN_BLOCK; p++) {
    //         int q = rand() % RECORDS_IN_BLOCK;

    //         struct record tmp;
    //         tmp = b1.records[p];
    //         b1.records[p] = b1.records[q];
    //         b1.records[q] = tmp;
    //     }

    //     status += write_block(&file, j, &b1);
    //     status += write_block(&file, i, &b2);

    //     if (status < 0)
    //         printf("%s: error shuffling blocks\n", __func__);
    // }

    // TODO randomness test
    for (int i = 0; i < blocks_to_generate * 2; i++) {
        int index1 = rand() % blocks_to_generate;
        int index2 = rand() % blocks_to_generate;

        struct block b1;
        struct block b2;

        int status = read_block(&file, index1, &b1);
        status += read_block(&file, index2, &b2);

        for (int p = 0; p < RECORDS_IN_BLOCK; p++) {
            int q = rand() % RECORDS_IN_BLOCK;

            struct record tmp;
            tmp = b1.records[p];
            b1.records[p] = b1.records[q];
            b1.records[q] = tmp;
        }

        for (int p = 0; p < RECORDS_IN_BLOCK; p++) {
            int q = rand() % RECORDS_IN_BLOCK;

            struct record tmp;
            tmp = b2.records[p];
            b2.records[p] = b2.records[q];
            b2.records[q] = tmp;
        }

        status += write_block(&file, index2, &b1);
        status += write_block(&file, index1, &b2);
    }

    fclose(file);
    return 0;
}

/*
 * Returns number of records that were entered
 *
 */
int disk_keyboard_input(const char *path) {
    int to_input;

    printf("Specify the number of records you are about to enter: \n");

    scanf("%d", &to_input);

    if (to_input <= 0)
        return 0;

    struct tape input_tape;
    input_tape.path = path;
    disk_open_file(&input_tape, "wb+");

    printf("Enter the records in the following format:\n");
    printf("  ABC 12345 John Doe 54\n");
    for (int i = 0; i < to_input; i++) {
        struct record new_record;
        scanf("%s", new_record.id.identity_series);
        scanf("%d", &new_record.id.identity_number);
        scanf("%s", new_record.data.name);
        scanf("%s", new_record.data.surname);
        scanf("%hd", &new_record.data.age);

        disk_append_record(&input_tape, &new_record);
    }

    disk_close_file(&input_tape);

    return to_input;
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
        //printf("%s: end of file reached\n", __func__); // TODO remove this message
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

    disk_num_write_operations += 1;

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

    disk_num_read_operations += 1;

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
    printf("buffer block = %d, record = %d\n", tape->buffer.block_index, tape->buffer.record_index);
    for (int i = 0; i < file_size(&tape->file) / BLOCK_SIZE; i++) {
        print_block(&tape->file, i);
    }

    if (tape->buffer.block == NULL)
        return;

    printf(" Buffer:\n");
    for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
        record_print(&tape->buffer.block->records[i], RECORD_PRINT_ID | RECORD_PRINT_NAME);
    }
}

/*
 * Print records on tape for debugging purposes
 *
 * Also highlights each run
 * 
 */
void disk_debug_tape(struct tape *tape) {
    printf("\n+++ %s:  runs = %d dummy = %d +++\n\n", tape->path, tape->num_runs, tape->dummy_runs);

    struct record prev;
    generate_incorrect_record(&prev);

    for (int i = 0; i < file_size(&tape->file) / BLOCK_SIZE; i++) {
        struct block block_to_read;
        int status = read_block(&tape->file, i, &block_to_read);
        if (status < 0) {
            printf("%s: error reading block\n", __func__);
        }
        for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
            if (record_compare(&block_to_read.records[i], &prev) < 0 && !record_is_empty(&block_to_read.records[i])) {
                printf("-*-\n");
            }

            record_print(&block_to_read.records[i], RECORD_PRINT_ID);

            prev = block_to_read.records[i];
        }
    }

    if (tape->buffer.block == NULL || tape->buffer.buffer_records == 0) {
        printf("\n+++ %s +++\n\n", tape->path);
        return;
    }

    for (int i = 0; i < RECORDS_IN_BLOCK; i++) {
        if (record_compare(&tape->buffer.block->records[i], &prev) < 0) {
            printf("--*--\n");
        }

        record_print(&tape->buffer.block->records[i], RECORD_PRINT_ID);

        prev = tape->buffer.block->records[i];
    }

    printf("\n+++ %s +++\n\n", tape->path);
}
