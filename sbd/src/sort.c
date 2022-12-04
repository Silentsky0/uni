#include "sort.h"
#include "common/status.h"
#include "record.h"
#include "disk.h"
#include <stdio.h>
#include "common/utils.h"

#define DISTRIBUTION_TAPE_1 0
#define DISTRIBUTION_TAPE_2 1
#define INITIAL_MERGE_TAPE  2

struct tape tapes[TAPES_NUM];
int return_merge_tape = -1;
int return_sorting_phases = 0;

void init_sort_tapes();
void close_sort_tapes();
int predicate_tapes_sorted();
void sort_single_phase(int runs_to_merge, int tape_1, int tape_2, int merge_tape);
void reopen_sort_tapes();
void sort_merge_record(struct record *record, int record_number, int from_tape, int merge_tape);
void sort_print_merge_records(struct record *r1, struct record *r2);
void open_sort_tape(int tape_num, const char *mode);
void close_sort_tape(int tape_num);
void sort_merge_dummy_runs(int runs_to_merge, int tape_num, int merge_tape);

void toggle_tape_index(int *tape_index) {
    *tape_index = 1 - *tape_index;
}

int sort_distribution_phase(const char *input_file_path, int print_contents) {
    
    // TODO some sort of init_input function
    struct tape input;
    input.path = input_file_path;
    input.file_mode = "rb";
    
    int status = disk_open_file(&input, input.file_mode);
    if (status < 0) {
        printf("%s: some error opening input file\n", __func__);
        return -1;
    }

    int fib_prev = 1;
    int fib_next = 1;

    struct record record;
    struct record prev;
    generate_incorrect_record(&prev);

    init_sort_tapes("wb+");

    int active_tape = 0;
    int num_runs = 1;

    printf("\n=== General distribution phase ===\n\n");

    if (print_contents)
        disk_debug_tape(&input);

    while (disk_get_next_record(&input, &record) > 0) {

        if(record_is_empty(&record))
            continue;

        // calculate runs in input file
        if (record_compare(&record, &prev) < 0) { 
            num_runs += 1;
        }

        // printf("\nRead record ");
        // record_print(&record, RECORD_PRINT_ID);

        // printf("\nBefore:\n");
        // printf("Active tape = %d Fib prev = %d, Fib next = %d\n", active_tape, fib_prev, fib_next);
        // //disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
        // //disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);
        // disk_debug_tape(&tapes[DISTRIBUTION_TAPE_1]);
        // disk_debug_tape(&tapes[DISTRIBUTION_TAPE_2]);

        // end of run or beginning of distribution
        if (record_compare(&record, &tapes[active_tape].last_record) < 0 || record_is_empty(&tapes[active_tape].last_record)) {
            tapes[active_tape].num_runs += 1;
            //printf("Tape %d now has %d runs\n", active_tape, tapes[active_tape].num_runs);
        }

        disk_append_record(&tapes[active_tape], &record);
        tapes[active_tape].last_record = record;
        tapes[active_tape].num_records += 1;

        if (tapes[active_tape].num_runs == fib_next) {
            toggle_tape_index(&active_tape);
            //printf("Toggle tape, increment fiboanacci numbers\n");

            // increment fibonacci numbers
            int tmp = fib_next;
            fib_next = fib_next + fib_prev;
            fib_prev = tmp;
        }

        // printf("\nAfter:\n");
        // printf("Active tape = %d Fib prev = %d, Fib next = %d\n", active_tape, fib_prev, fib_next);
        // //disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
        // disk_debug_tape(&tapes[DISTRIBUTION_TAPE_1]);
        // //disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);
        // disk_debug_tape(&tapes[DISTRIBUTION_TAPE_2]);
        // printf("\n");

        prev = record;
    }

    if (tapes[active_tape].num_runs != fib_next - fib_prev)
        tapes[active_tape].dummy_runs = fib_next - tapes[active_tape].num_runs;

    //printf("Tape %d dummy runs = %d\n", active_tape, tapes[active_tape].dummy_runs);

    // disk_debug_tape(&tapes[0]);
    // disk_debug_tape(&tapes[1]);

    disk_close_file(&input);
    close_sort_tapes();

    printf("Number of runs on input file: %d\n", num_runs);

    printf("Number of disk operations:\n");
    printf("    read  %d\n", get_read_operations_number());
    printf("    write %d\n\n", get_write_operations_number());

    return 0;
}

int sort_sorting_phase(int print_contents, int print_every_phase) {

    printf("\n=== Sorting phase ===\n\n");

    int phase_number = 0;

    int tape_1 = DISTRIBUTION_TAPE_1;
    int tape_2 = DISTRIBUTION_TAPE_2;
    int merge_tape = INITIAL_MERGE_TAPE;

    //int tape_1_runs = tapes[tape_1].num_runs;
    //int tape_2_runs = tapes[tape_2].num_runs;

    printf("tape 1 runs = %d, tape 2 runs = %d, sum = %d\n", tapes[tape_1].num_runs, tapes[tape_2].num_runs, tapes[tape_1].num_runs + tapes[tape_2].num_runs);

    int dummy_tape = 0;
    int dummy_runs = 0;

    if (tapes[tape_1].dummy_runs != 0) {
        dummy_tape = tape_2;
        dummy_runs = tapes[tape_1].dummy_runs;
    }
    else {
        dummy_tape = tape_1;
        dummy_runs = tapes[tape_2].dummy_runs;
    }

    open_sort_tape(tape_1, "rb+");
    open_sort_tape(tape_2, "rb+");
    open_sort_tape(merge_tape, "wb+");

    // DEBUG
    // printf("\n\n\n\n===== D E B U G =====\n\n");
    // disk_debug_tape(&tapes[tape_1]);
    // disk_debug_tape(&tapes[tape_2]);
    // printf("\n\n\n");


    sort_merge_dummy_runs(dummy_runs, dummy_tape, merge_tape);

    while (!predicate_tapes_sorted()) {
        // TODO remove
        // if (phase_number == 6)
        //     break;

        int tape_1_runs = tapes[tape_1].num_runs;
        int tape_2_runs = tapes[tape_2].num_runs;

        // begin one sorting phase 

        int runs_to_merge = min(tapes[tape_1].num_runs, tapes[tape_2].num_runs);

        //printf("\n\n=== Sorting phase %d merge %d runs===\n\n", phase_number, runs_to_merge);

        //disk_debug_tape(&tapes[tape_1]);
        //disk_debug_tape(&tapes[tape_2]);
        //disk_debug_tape(&tapes[merge_tape]);

        sort_single_phase(runs_to_merge, tape_1, tape_2, merge_tape);
        dummy_runs = 0;

        if (print_every_phase) {
            printf("\n== Sorting phase %d ==\n", phase_number);
            disk_debug_tape(&tapes[0]);
            disk_debug_tape(&tapes[1]);
            disk_debug_tape(&tapes[2]);
        }

        if (!predicate_tapes_sorted()) {
            // swap tape numbers
            if (tape_1_runs > tape_2_runs) {
                close_sort_tape(tape_2);
                close_sort_tape(merge_tape);
    
                int tmp = tape_2;
                tape_2 = merge_tape;
                merge_tape = tmp;
    
                open_sort_tape(tape_2, "rb+");
                open_sort_tape(merge_tape, "wb+");
    
                //tapes[tape_2].num_runs = runs_to_merge + dummy_runs;
                //tapes[merge_tape].num_runs = 0;
            }else {
                close_sort_tape(tape_1);
                close_sort_tape(merge_tape);
    
                int tmp = tape_1;
                tape_1 = merge_tape;
                merge_tape = tmp;
    
                open_sort_tape(tape_1, "rb+");
                open_sort_tape(merge_tape, "wb+");
    
                //tapes[tape_1].num_runs = runs_to_merge + dummy_runs;
                //[merge_tape].num_runs = 0;
            }
        }

        // end sorting phase
        phase_number += 1;
        return_sorting_phases += 1;
    }

    //printf("\nSORTING DONE\n\n");
    //disk_debug_tape(&tapes[tape_1]);
    //disk_debug_tape(&tapes[tape_2]);
    //disk_debug_tape(&tapes[merge_tape]);
    return_merge_tape = merge_tape;
    
    if (print_contents)
        disk_debug_tape(&tapes[return_merge_tape]);

    close_sort_tapes();

    return 0;
}

void sort_merge_dummy_runs(int runs_to_merge, int tape_num, int merge_tape) {

    int runs_merged = 0;

    //printf("=== merge %d dummy runs ===\n", runs_to_merge);

    struct record record_merge;
    struct record previous_record;
    generate_incorrect_record(&previous_record);

    while (runs_merged < runs_to_merge) {

        int tape_run_done = 0;

        if (record_is_empty(&tapes[tape_num].extra_record)) {
            disk_get_next_record(&tapes[tape_num], &record_merge);
        } else {
            record_merge = tapes[tape_num].extra_record;
            generate_incorrect_record(&tapes[tape_num].extra_record);
        }

        // merge one run
        while (!tape_run_done) {

            previous_record = record_merge;
            sort_merge_record(&record_merge, tape_num, tape_num, merge_tape);

            // record 1 ends run
            if (record_compare(&record_merge, &previous_record) < 0 || record_is_empty(&record_merge)) {
                tape_run_done = 1;
                tapes[tape_num].num_runs -= 1;
                tapes[tape_num].extra_record = record_merge;

                //printf("extra record dummy is now ");
                //record_print(&tapes[tape_num].extra_record, RECORD_PRINT_ID);

                generate_incorrect_record(&record_merge);

                //printf("Dummy: End of run on tape %d\n", tape_num);
            }
        }

        runs_merged += 1;
    }
    tapes[merge_tape].num_runs = runs_to_merge;

    tapes[0].dummy_runs = 0;
    tapes[1].dummy_runs = 0;
    tapes[2].dummy_runs = 0;
    //disk_debug_tape(&tapes[merge_tape]);
}

void sort_single_phase(int runs_to_merge, int tape_1, int tape_2, int merge_tape) {

    //tapes[merge_tape].num_runs += 1;

    int runs_merged = 0;

    struct record record_merge_1;
    struct record record_merge_2;

    struct record previous_record_1;
    struct record previous_record_2;
    generate_incorrect_record(&previous_record_1);
    generate_incorrect_record(&previous_record_2);

    while (runs_merged < runs_to_merge) {

        int tape_1_run_done = 0;
        int tape_2_run_done = 0;

        // initialise
        // pull from tape 1
        if (record_is_empty(&tapes[tape_1].extra_record)) {
            disk_get_next_record(&tapes[tape_1], &record_merge_1);
        } else {
            record_merge_1 = tapes[tape_1].extra_record;
            generate_incorrect_record(&tapes[tape_1].extra_record);
        }
        // pull from tape 2
        if (record_is_empty(&tapes[tape_2].extra_record)) {
            disk_get_next_record(&tapes[tape_2], &record_merge_2);
        } else {
            record_merge_2 = tapes[tape_2].extra_record;
            generate_incorrect_record(&tapes[tape_2].extra_record);
        }

        // merge 1 run from both tapes
        //printf("--- merge 1 run from both tapes, %d to go\n\n", runs_to_merge - runs_merged);
        while (!(tape_1_run_done && tape_2_run_done)) {
            
            //printf("-test-\n");
            //sort_print_merge_records(&record_merge_1, &record_merge_2);

            // record 1 is smaller
            if (record_compare(&record_merge_1, &record_merge_2) < 0 && !tape_1_run_done) {
                //printf("comparison: record 1 smaller\n");
                previous_record_1 = record_merge_1;
                sort_merge_record(&record_merge_1, 1, tape_1, merge_tape);

                //sort_print_merge_records(&record_merge_1, &record_merge_2);

                // record 1 ends run
                if (record_compare(&record_merge_1, &previous_record_1) < 0 || record_is_empty(&record_merge_1)) { // TODO this clause could be a function
                    tape_1_run_done = 1;
                    tapes[tape_1].num_runs -= 1;
                    tapes[tape_1].extra_record = record_merge_1;

                    //printf("extra record 1 is now ");
                    //record_print(&tapes[tape_1].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_1);

                    //printf("End of run on tape %d\n", tape_1);
                }
            }

            // record 2 is smaller
            else if (record_compare(&record_merge_1, &record_merge_2) > 0 && !tape_2_run_done) {
                //printf("comparison: record 2 smaller\n");
                previous_record_2 = record_merge_2;
                sort_merge_record(&record_merge_2, 2, tape_2, merge_tape);

                //sort_print_merge_records(&record_merge_1, &record_merge_2);

                //record 2 ends run
                if (record_compare(&record_merge_2, &previous_record_2) < 0 || record_is_empty(&record_merge_2)) {
                    tape_2_run_done = 1;
                    tapes[tape_2].num_runs -= 1;
                    tapes[tape_2].extra_record = record_merge_2;

                    //printf("extra record 2 is now ");
                    //record_print(&tapes[tape_2].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_2);

                    //printf("End of run on tape %d\n", tape_2);
                }
            }

            // record 1 is empty
            if (record_is_empty(&record_merge_1)) {
                //printf("record 1 is empty\n");
                previous_record_2 = record_merge_2;
                sort_merge_record(&record_merge_2, 2, tape_2, merge_tape);

                //sort_print_merge_records(&record_merge_1, &record_merge_2);

                //record 2 ends run
                if (record_compare(&record_merge_2, &previous_record_2) < 0 || record_is_empty(&record_merge_2)) {
                    tape_2_run_done = 1;
                    tapes[tape_2].num_runs -= 1;
                    tapes[tape_2].extra_record = record_merge_2;

                    //printf("extra record 2 is now ");
                    //record_print(&tapes[tape_2].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_2);

                    //printf("End of run on tape %d\n", tape_2);
                }
            }

            // record 2 is empty
            else if (record_is_empty(&record_merge_2)) {
                //printf("record 2 is empty\n");
                previous_record_1 = record_merge_1;
                sort_merge_record(&record_merge_1, 1, tape_1, merge_tape);

                //sort_print_merge_records(&record_merge_1, &record_merge_2);

                // record 1 ends run
                if (record_compare(&record_merge_1, &previous_record_1) < 0 || record_is_empty(&record_merge_1)) {
                    tape_1_run_done = 1;
                    tapes[tape_1].num_runs -= 1;
                    tapes[tape_1].extra_record = record_merge_1;


                    //printf("extra record 1 is now ");
                    //record_print(&tapes[tape_1].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_1);

                    //printf("End of run on tape %d\n", tape_1);
                }
            }

            // if (record_is_empty(&record_merge_1) && record_is_empty(&record_merge_1)) {
            //     printf("this situation also happened\n");
            //     disk_debug_tape(&tapes[merge_tape]);
            //     break;
            // }

            //printf("run1 done = %d, run2 done = %d", tape_1_run_done, tape_2_run_done);
            //disk_debug_tape(&tapes[merge_tape]);
            //printf("\n\n");
        }

        runs_merged += 1;
        tapes[merge_tape].num_runs += 1;
    }



    //printf("\nAfter:\n");
    //disk_debug_tape(&tapes[merge_tape]);
    // printf("tape 1 block %d record %d\n", tapes[tape_1].buffer.block_index, tapes[tape_1].buffer.record_index);
    // printf("tape 2 block %d record %d\n", tapes[tape_2].buffer.block_index, tapes[tape_2].buffer.record_index);
    
    
    // struct record rec;
    // printf("tape 1:\n");
    // record_print(&tapes[tape_1].extra_record, RECORD_PRINT_ID);
    // printf("\n");
    // while (disk_get_next_record(&tapes[tape_1], &rec) >= 0) {
    //     record_print(&rec, RECORD_PRINT_EMPTY_RECORDS | RECORD_PRINT_ID);
    // }
    // printf("tape 2:\n");
    // record_print(&tapes[tape_2].extra_record, RECORD_PRINT_ID);
    // printf("\n");
    // while (disk_get_next_record(&tapes[tape_2], &rec) >= 0) {
    //     record_print(&rec, RECORD_PRINT_EMPTY_RECORDS | RECORD_PRINT_ID);
    // }
    // disk_print_file(&tapes[tape_1]);
    // disk_print_file(&tapes[tape_2]);
    // disk_print_file(&tapes[merge_tape]);
}

void sort_postprocess_phase(int initial_elements_num, int print_contents) {
    open_sort_tape(return_merge_tape, "rb+");

    printf("\n=== Postprocess phase ===\n\n");

    printf("Number of sorting phases: %d\n\n", return_sorting_phases);

    printf("Number of disk operations:\n");
    printf("    read  %d\n", get_read_operations_number());
    printf("    write %d\n", get_write_operations_number());

    struct record rec;
    struct record prev;
    generate_incorrect_record(&prev);

    int sorting_errors = 0;
    int num_of_elements = 0;

    while (disk_get_next_record(&tapes[return_merge_tape], &rec) >= 0) {
        if (record_is_empty(&rec)) {
            if (print_contents)
                record_print(&rec, RECORD_PRINT_ID | RECORD_PRINT_EMPTY_RECORDS);
            continue;
        }

        if (print_contents)
            record_print(&rec, RECORD_PRINT_EMPTY_RECORDS | RECORD_PRINT_ID);
        num_of_elements += 1;

        if (record_compare(&rec, &prev) < 0 && !record_is_equal(&rec, &prev)) { // prev is bigger, incorrect sorting
            sorting_errors += 1;
            if (print_contents) 
                printf("\nERROR: tape is not sorted properly\n\n");
        }

        if (record_is_equal(&rec, &prev)) {
            printf("records are equal:  !!!\n");
            record_print(&prev, RECORD_PRINT_ID | RECORD_PRINT_EMPTY_RECORDS);
        }

        prev = rec;
    }

    // keep in mind that generated number of elements is rounded up to fill up
    // the last block
    int initial_elements_num_corrected = (initial_elements_num / RECORDS_IN_BLOCK + 1) * RECORDS_IN_BLOCK;

    if (initial_elements_num_corrected != num_of_elements && print_contents) {
        printf("\nERROR: there is a different number of records %d != %d\n\n", initial_elements_num_corrected, num_of_elements);
    }

    if (num_of_elements == initial_elements_num_corrected && sorting_errors == 0) {
        printf("Everything is correct\n");
    }
    else {
        printf("Some errors occured: elements_num %d != %d, sorting errors = %d\n", initial_elements_num_corrected, num_of_elements, sorting_errors);
    }

    close_sort_tape(return_merge_tape);
}

int predicate_tapes_sorted() {
    int sum = 0;
    for (int i = 0; i < TAPES_NUM; i++) {
        sum += tapes[i].num_runs + tapes[i].dummy_runs;
    }
    return sum == 1;
}

void sort_merge_record(struct record *record, int record_number, int from_tape, int merge_tape) {
    //printf("Merging record %d: ", record_number);
    //record_print(record, RECORD_PRINT_ID);

    disk_append_record(&tapes[merge_tape], record);

    disk_get_next_record(&tapes[from_tape], record);

    //printf("new record %d: ", record_number);
    //record_print(record, RECORD_PRINT_ID);
    //printf("\n");
}

void sort_print_merge_records(struct record *r1, struct record *r2) {
    printf("Record 1 : ");
    record_print(r1, RECORD_PRINT_ID | RECORD_PRINT_EMPTY_RECORDS);
    printf("\nRecord 2 : ");
    record_print(r2, RECORD_PRINT_ID | RECORD_PRINT_EMPTY_RECORDS);
    printf("\n");
}

/*
 * DEPRECATED - useless function
 */
void sort_remove_duplicates_phase(const char *path, const char *dest) {
    struct tape input;
    input.path = path;
    struct tape destination;
    destination.path = dest;

    int status = disk_open_file(&input, "rb");
    if (status < 0) {
        printf("%s: some error opening file %s\n", __func__, input.path);
    }
    status = disk_open_file(&destination, "rb");
    if (status < 0) {
        printf("%s: some error opening file %s\n", __func__, destination.path);
    }

    struct record rec;
    struct record prev;
    generate_incorrect_record(&prev);

    while (disk_get_next_record(&tapes[return_merge_tape], &rec) >= 0) {

        if (!record_is_equal(&rec, &prev)) {
            disk_append_record(&destination, &rec);
        }

        prev = rec;
    }

    disk_close_file(&input);
    disk_close_file(&destination);
}

void init_sort_tapes(const char *mode) {
    tapes[0].path = TAPE_0_DIR;
    tapes[1].path = TAPE_1_DIR;
    tapes[2].path = TAPE_2_DIR;

    for (int i = 0; i < TAPES_NUM; i++) {
        tapes[i].num_runs = 0;
        tapes[i].num_records = 0;
        tapes[i].dummy_runs = 0;
        tapes[i].file_mode = mode;
        generate_incorrect_record(&tapes[i].last_record);
        generate_incorrect_record(&tapes[i].extra_record);

        int status = disk_open_file(&tapes[i], tapes[i].file_mode);
        if (status < 0) {
            printf("%s: some error opening tape %d file\n", __func__, i);
            return;
        }
    }
}

void reopen_sort_tapes() {
    for (int i = 0; i < TAPES_NUM; i++) {
        int status = disk_open_file(&tapes[i], "rb+");
        if (status < 0) {
            printf("%s: some error opening tape %d file\n", __func__, i);
            return;
        }
    }
}

void close_sort_tapes() {
    for (int i = 0; i < TAPES_NUM; i++) { 
        disk_close_file(&tapes[i]);
    }
}

void open_sort_tape(int tape_num, const char *mode) {
    disk_open_file(&tapes[tape_num], mode);
}

void close_sort_tape(int tape_num) {
    disk_close_file(&tapes[tape_num]);
}
