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
    init_sort_tapes();

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
        disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
        disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);

        if (record_compare(&record, &tapes[active_tape].last_record) < 0) {
            tapes[active_tape].num_runs += 1;
            printf("Tape %d now has %d runs\n", active_tape, tapes[active_tape].num_runs);
        }

        if (tapes[active_tape].num_runs == fib_next) {
            toggle_tape_index(&active_tape);
            printf("Toggle tape, increment fibonacci numbers\n");
            // increment fibonacci numbers
            int tmp = fib_next;
            fib_next = fib_next + fib_prev;
            fib_prev = tmp;
        }

        disk_append_record(&tapes[active_tape], &record);
        tapes[active_tape].last_record = record;
        tapes[active_tape].num_records += 1;

        printf("\nAfter:\n");
        printf("Active tape = %d Fib prev = %d, Fib next = %d\n", active_tape, fib_prev, fib_next);
        disk_print_file(&tapes[DISTRIBUTION_TAPE_1]);
        disk_print_file(&tapes[DISTRIBUTION_TAPE_2]);
        printf("\n");
    }

    tapes[active_tape].num_runs += 1;

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

    init_sort_tapes();

    while (!predicate_tapes_sorted()) {
        // begin one sorting phase 

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

    static int phase_counter = 0;

    printf("\n\n=== Sorting phase %d ===\n\n", phase_counter);
    printf("Before:\n");
    disk_print_file(&tapes[tape_1]);
    disk_print_file(&tapes[tape_2]);
    disk_print_file(&tapes[merge_tape]);

    int runs_to_merge = min(tapes[tape_1].num_runs, tapes[tape_1].num_runs);

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
            disk_get_next_record(&tapes[tape_2], &record_merge_1);
        } else {
            record_merge_1 = tapes[tape_2].extra_record;
            generate_incorrect_record(&tapes[tape_2].extra_record);
        }

        // merge 1 run from both tapes
        while (!tape_1_run_done && !tape_2_run_done) {
            
            // record 1 is smaller
            if (record_compare(&record_merge_1, &record_merge_2) < 0) {
                disk_append_record(&tapes[merge_tape], &record_merge_1);
                disk_get_next_record(&tapes[tape_1], &record_merge_1);
            }

            // record 2 is smaller
            if (record_compare(&record_merge_1, &record_merge_2) > 0) {
                disk_append_record(&tapes[merge_tape], &record_merge_2);
                disk_get_next_record(&tapes[tape_2], &record_merge_2);
            }

            // record 1 ends run
            if (record_compare(&record_merge_1, &previous_record_1) < 0) {
                tape_1_run_done = 1;
                tapes[tape_1].extra_record = record_merge_1;
            }

            //record 2 ends run
            if (record_compare(&record_merge_2, &previous_record_2) < 0) {
                tape_2_run_done = 1;
                tapes[tape_2].extra_record = record_merge_2;
            }
        }

        runs_merged += 1;
    }

    printf("After:\n");
    disk_print_file(&tapes[tape_1]);
    disk_print_file(&tapes[tape_2]);
    disk_print_file(&tapes[merge_tape]);
}

int predicate_tapes_sorted() {
    int sum = 0;
    for (int i = 0; i < TAPES_NUM; i++) {
        sum += tapes[i].num_runs + tapes[i].dummy_runs;
    }
    return sum == 1;
}

void init_sort_tapes() {
    tapes[0].path = TAPE_0_DIR;
    tapes[1].path = TAPE_1_DIR;
    tapes[2].path = TAPE_2_DIR;

    for (int i = 0; i < TAPES_NUM; i++) {
        tapes[i].num_runs = 0;
        tapes[i].num_records = 0;
        tapes[i].dummy_runs = 0;
        tapes[i].file_mode = "wb+";
        generate_incorrect_record(&tapes[i].last_record);
        generate_incorrect_record(&tapes[i].extra_record);

        int status = disk_open_file(&tapes[i], tapes[i].file_mode);
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
