#include "sort.h"
#include "common/status.h"
#include "record.h"
#include "disk.h"
#include <stdio.h>

struct tape tapes[TAPES_NUM];

void init_tapes();

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
    init_tapes();

    int active_tape = 0;

    disk_print_file(&tapes[0]);
    disk_print_file(&tapes[1]);

    printf("\n=== General distribution phase ===\n\n");

    while (disk_get_next_record(&input, &record) > 0) {

        if(record_is_empty(&record))
            continue;

        printf("\nRead record ");
        record_print(&record, RECORD_PRINT_ID);

        printf("\nBefore:\n");
        printf("Active tape = %d Fib prev = %d, Fib next = %d\n", active_tape, fib_prev, fib_next);
        disk_print_file(&tapes[0]);
        disk_print_file(&tapes[1]);

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
        disk_print_file(&tapes[0]);
        disk_print_file(&tapes[1]);
        printf("\n");
    }

    // set dummy runs
    if (tapes[active_tape].num_runs != fib_prev) 
        tapes[active_tape].dummy_runs = fib_next - tapes[active_tape].num_runs;

    printf("Tape %d dummy runs = %d\n", active_tape, tapes[active_tape].dummy_runs);

    disk_close_file(&input);
    disk_close_file(&tapes[0]);
    disk_close_file(&tapes[1]);

    return 0;
}

void init_tapes() {
    tapes[0].path = TAPE_0_DIR;
    tapes[1].path = TAPE_1_DIR;
    tapes[2].path = TAPE_2_DIR;

    for (int i = 0; i < TAPES_NUM; i++) {
        tapes[i].num_runs = 0;
        tapes[i].num_records = 0;
        tapes[i].dummy_runs = 0;
        tapes[i].file_mode = "wb+";
        generate_incorrect_record(&tapes[i].last_record);

        int status = disk_open_file(&tapes[i], tapes[i].file_mode);
        if (status < 0) {
            printf("%s: some error opening tape %d file\n", __func__, i);
            return;
        }
    }
}
