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

void init_sort_tapes();
void close_sort_tapes();
int predicate_tapes_sorted();
void sort_single_phase(int tape_1, int tape_2, int merge_tape);
void reopen_sort_tapes();
void sort_merge_record(struct record *record, int record_number, int from_tape, int merge_tape);
void sort_print_merge_records(struct record *r1, struct record *r2);

void toggle_tape_index(int *tape_index) {
    *tape_index = 1 - *tape_index;
}

int sort_distribution_phase(const char *input_file_path) {
    
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
    init_sort_tapes("wb+");

    int active_tape = 0;

    disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
    disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);

    printf("\n=== General distribution phase ===\n\n");

    while (disk_get_next_record(&input, &record) > 0) {

        if(record_is_empty(&record))
            continue;

        printf("\nRead record ");
        record_print(&record, RECORD_PRINT_ID);

        printf("\nBefore:\n");
        printf("Active tape = %d Fib prev = %d, Fib next = %d\n", active_tape, fib_prev, fib_next);
        //disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
        //disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);
        disk_debug_tape(&tapes[DISTRIBUTION_TAPE_1]);
        disk_debug_tape(&tapes[DISTRIBUTION_TAPE_2]);

        // end of run or beginning of distribution
        if (record_compare(&record, &tapes[active_tape].last_record) < 0 || record_is_empty(&tapes[active_tape].last_record)) {
            tapes[active_tape].num_runs += 1;
            printf("Tape %d now has %d runs\n", active_tape, tapes[active_tape].num_runs);
        }

        disk_append_record(&tapes[active_tape], &record);
        tapes[active_tape].last_record = record;
        tapes[active_tape].num_records += 1;

        if (tapes[active_tape].num_runs == fib_next) {
            toggle_tape_index(&active_tape);
            printf("Toggle tape, increment fibonacci numbers\n");

            // increment fibonacci numbers
            int tmp = fib_next;
            fib_next = fib_next + fib_prev;
            fib_prev = tmp;
        }

        printf("\nAfter:\n");
        printf("Active tape = %d Fib prev = %d, Fib next = %d\n", active_tape, fib_prev, fib_next);
        //disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
        disk_debug_tape(&tapes[DISTRIBUTION_TAPE_1]);
        //disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);
        disk_debug_tape(&tapes[DISTRIBUTION_TAPE_2]);
        printf("\n");
    }

    //tapes[active_tape].num_runs += 1;

    // set dummy runs
    //if (tapes[active_tape].num_runs != fib_prev)  // TODO this might be wrong
    tapes[active_tape].dummy_runs = fib_next - tapes[active_tape].num_runs;

    printf("Tape %d dummy runs = %d\n", active_tape, tapes[active_tape].dummy_runs);

    // TODO after a perfect distribution, sometimes the number of dummy runs is
    // wrong - this might have already been fixed

    disk_close_file(&input);
    close_sort_tapes();

    return 0;
}

int sort_sorting_phase() {

    int phase_number = 0;

    int tape_1 = DISTRIBUTION_TAPE_1;
    int tape_2 = DISTRIBUTION_TAPE_2;
    int merge_tape = INITIAL_MERGE_TAPE;

    int tape_1_runs = tapes[tape_1].num_runs;
    int tape_2_runs = tapes[tape_2].num_runs;

    reopen_sort_tapes();

    // DEBUG
    printf("\n\n\n\n===== D E B U G =====\n\n");
    disk_debug_tape(&tapes[tape_1]);
    disk_debug_tape(&tapes[tape_2]);
    printf("\n\n\n");

    tapes[tape_1].num_runs += tapes[tape_1].dummy_runs;
    tapes[tape_2].num_runs += tapes[tape_2].dummy_runs;


    while (!predicate_tapes_sorted()) {
        // TODO remove
        if (phase_number == 3)
            break;

        // begin one sorting phase 

        printf("\n\n=== Sorting phase %d ===\n\n", phase_number);

        sort_single_phase(tape_1, tape_2, merge_tape);


        // swap tape numbers
        if (tape_1_runs > tape_2_runs) {
            int tmp_1 = tape_1;
            int tmp_2 = tape_2;
            tape_2 = merge_tape;
            tape_1 = tmp_2;
            merge_tape = tmp_1;
        }else {
            int tmp_1 = tape_1;
            int tmp_2 = tape_2;
            tape_1 = merge_tape;
            tape_2 = tmp_1;
            merge_tape = tmp_2;
        }

        // end sorting phase
        phase_number += 1;
    }

    return 0;
}

void sort_single_phase(int tape_1, int tape_2, int merge_tape) {

    // printf("tape1 = %d tape2 = %d merge_tape = %d Before:\n", tape_1, tape_2, merge_tape);
    // disk_print_file(&tapes[tape_1]);
    // disk_print_file(&tapes[tape_2]);
    // disk_print_file(&tapes[merge_tape]);

    int runs_to_merge = min(tapes[tape_1].num_runs, tapes[tape_2].num_runs);

    int runs_merged = 0;

    printf("\nRuns to merge: %d\n\n", runs_to_merge);

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
        printf("--- merge 1 run from both tapes, %d to go\n\n", runs_to_merge - runs_merged);
        while (!(tape_1_run_done && tape_2_run_done)) {
            
            printf("-test-\n");
            sort_print_merge_records(&record_merge_1, &record_merge_2);

            // record 1 is smaller
            if (record_compare(&record_merge_1, &record_merge_2) < 0 && !tape_1_run_done) {
                printf("comparison: record 1 smaller\n");
                previous_record_1 = record_merge_1;
                sort_merge_record(&record_merge_1, 1, tape_1, merge_tape);

                sort_print_merge_records(&record_merge_1, &record_merge_2);

                // record 1 ends run
                if (record_compare(&record_merge_1, &previous_record_1) < 0 || record_is_empty(&record_merge_1)) { // TODO this clause could be a function
                    tape_1_run_done = 1;
                    tapes[tape_1].num_runs -= 1;
                    tapes[tape_1].extra_record = record_merge_1;

                    printf("extra record 1 is now ");
                    record_print(&tapes[tape_1].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_1);

                    printf("End of run on tape %d\n", tape_1);
                }
            }

            // record 2 is smaller
            else if (record_compare(&record_merge_1, &record_merge_2) > 0 && !tape_2_run_done) {
                printf("comparison: record 2 smaller\n");
                previous_record_2 = record_merge_2;
                sort_merge_record(&record_merge_2, 2, tape_2, merge_tape);

                sort_print_merge_records(&record_merge_1, &record_merge_2);

                //record 2 ends run
                if (record_compare(&record_merge_2, &previous_record_2) < 0 || record_is_empty(&record_merge_2)) {
                    tape_2_run_done = 1;
                    tapes[tape_2].num_runs -= 1;
                    tapes[tape_2].extra_record = record_merge_2;

                    printf("extra record 2 is now ");
                    record_print(&tapes[tape_2].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_2);

                    printf("End of run on tape %d\n", tape_2);
                }
            }

            if (tape_1_run_done && tape_2_run_done) {
                printf("this situation happened\n");
                disk_debug_tape(&tapes[merge_tape]);
                break;
            }

            // record 1 is empty
            if (record_is_empty(&record_merge_1)) {
                printf("record 1 is empty\n");
                previous_record_2 = record_merge_2;
                sort_merge_record(&record_merge_2, 2, tape_2, merge_tape);

                sort_print_merge_records(&record_merge_1, &record_merge_2);

                //record 2 ends run
                if (record_compare(&record_merge_2, &previous_record_2) < 0 || record_is_empty(&record_merge_2)) {
                    tape_2_run_done = 1;
                    tapes[tape_2].num_runs -= 1;
                    tapes[tape_2].extra_record = record_merge_2;

                    printf("extra record 2 is now ");
                    record_print(&tapes[tape_2].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_2);

                    printf("End of run on tape %d\n", tape_2);
                }
            }

            // record 2 is empty
            else if (record_is_empty(&record_merge_2)) {
                printf("record 2 is empty\n");
                previous_record_1 = record_merge_1;
                sort_merge_record(&record_merge_1, 1, tape_1, merge_tape);

                sort_print_merge_records(&record_merge_1, &record_merge_2);

                // record 1 ends run
                if (record_compare(&record_merge_1, &previous_record_1) < 0 || record_is_empty(&record_merge_1)) {
                    tape_1_run_done = 1;
                    tapes[tape_1].num_runs -= 1;
                    tapes[tape_1].extra_record = record_merge_1;


                    printf("extra record 1 is now ");
                    record_print(&tapes[tape_1].extra_record, RECORD_PRINT_ID);

                    generate_incorrect_record(&record_merge_1);

                    printf("End of run on tape %d\n", tape_1);
                }
            }

            // if (record_is_empty(&record_merge_1) && record_is_empty(&record_merge_1)) {
            //     printf("this situation also happened\n");
            //     disk_debug_tape(&tapes[merge_tape]);
            //     break;
            // }

            printf("run1 done = %d, run2 done = %d", tape_1_run_done, tape_2_run_done);
            disk_debug_tape(&tapes[merge_tape]);
            printf("\n\n");
        }

        runs_merged += 1;
    }

    // printf("After:\n");
    // disk_print_file(&tapes[tape_1]);
    // disk_print_file(&tapes[tape_2]);
    // disk_print_file(&tapes[merge_tape]);
}

int predicate_tapes_sorted() {
    int sum = 0;
    for (int i = 0; i < TAPES_NUM; i++) {
        sum += tapes[i].num_runs + tapes[i].dummy_runs;
    }
    return sum == 1;
}

void sort_merge_record(struct record *record, int record_number, int from_tape, int merge_tape) {
    printf("Merging record %d: ", record_number);
    record_print(record, RECORD_PRINT_ID);

    disk_append_record(&tapes[merge_tape], record);

    disk_get_next_record(&tapes[from_tape], record);

    printf("new record %d: ", record_number);
    record_print(record, RECORD_PRINT_ID);
    printf("\n");
}

void sort_print_merge_records(struct record *r1, struct record *r2) {
    printf("Record 1 : ");
    record_print(r1, RECORD_PRINT_ID | RECORD_PRINT_EMPTY_RECORDS);
    printf("\nRecord 2 : ");
    record_print(r2, RECORD_PRINT_ID | RECORD_PRINT_EMPTY_RECORDS);
    printf("\n");
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
