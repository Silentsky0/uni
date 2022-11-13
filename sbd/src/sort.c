#include "sort.h"
#include "common/status.h"
#include "record.h"
#include "disk.h"
#include <stdio.h>

struct tape tapes[TAPES_NUM];

void init_tapes();

void toggle_tape_index(int *tape_index) {
    *tape_index ^= 1;
}

int sort_distribution_phase(const char *input_file_path) {
    
    struct tape input;
    input.path = input_file_path;
    
    int status = disk_open_file(&input, "rb");
    if (status < 0) {
        printf("%s: some error opening input file\n", __func__);
        return -1;
    }

    struct record record;
    //struct record previous_record;
    init_tapes();

    int tape_index = 0;

    // insert initial record
    //disk_get_next_record(&input, &record);
    //disk_append_record(&tapes[tape_index], &record);
    //tapes[0].num_runs = 1;
    //tapes[0].last_record = record;
    disk_print_file(&tapes[0]);
    disk_print_file(&tapes[1]);

    printf("\n=== General distribution phase ===\n\n");

    while (disk_get_next_record(&input, &record) > 0) {
        if (record_is_empty(&record))
            continue;

        printf("\nRead record ");
        record_print(&record, RECORD_PRINT_ID);

        printf("\nBefore:\n");
        disk_print_file(&tapes[0]);
        disk_print_file(&tapes[1]);


        // if previous record is bigger, end the run
        if (record_compare(&record, &tapes[tape_index].last_record) < 0) {
            tapes[tape_index].num_runs += 1;
            printf("Tape %d: end of run\n", tape_index);
        }

        if (tapes[tape_index].num_runs == tapes[tape_index].fib && tape_index == 0) {
            tape_index = 1;

            printf("Switching to append to tape %d now\n", tape_index);

            if (tapes[tape_index].num_runs == tapes[tape_index].fib) {
                tape_index = 0;
                int tmp = tapes[0].fib;
                tapes[0].fib = tapes[0].fib + tapes[1].fib;
                tapes[1].fib = tmp;
                printf("Resetting to tape %d now\n", tape_index);
            }
        }

        disk_append_record(&tapes[tape_index], &record);

        tapes[tape_index].last_record = record;
        
        if (tapes[tape_index].num_records == 0)
            tapes[tape_index].num_runs = 1;
        tapes[tape_index].num_records += 1;


        printf("\nAfter:\n");
        disk_print_file(&tapes[0]);
        disk_print_file(&tapes[1]);

        printf("\n");
    }

    disk_close_file(&input);

    return 0;
}

void init_tapes() {
    tapes[0].path = TAPE_0_DIR;
    tapes[1].path = TAPE_1_DIR;
    tapes[2].path = TAPE_2_DIR;

    for (int i = 0; i < TAPES_NUM; i++) {
        tapes[i].num_runs = 0;
        tapes[i].num_records = 0;

        int status = disk_open_file(&tapes[i], "wb+");
        if (status < 0) {
            printf("%s: some error opening tape %d file\n", __func__, i);
            return;
        }
    }

    tapes[0].fib = 1;
    tapes[1].fib = 1;
}
