#include "sort.h"
#include "common/status.h"
#include "record.h"
#include "disk.h"
#include <stdio.h>

struct tape tapes[TAPES_NUM];

void init_tapes();

int sort_distribution_phase(const char *input_file_path) {
    int fib_0 = 0;
    int fib_1 = 1;
    
    FILE *input;
    
    int status = disk_open_file(&input, input_file_path);
    if (status < 0) {
        printf("%s: some error opening input file\n", __func__);
        return -1;
    }

    struct record record;
    init_tapes();

    while (disk_get_next_record(&input, &record) > 0) {
        //printf("inserting record into tape\n");
        record_print(&record, RECORD_PRINT_ID | RECORD_PRINT_NAME);

        // while (tapes[0].num_runs <= fib_0) {

        // }

        // while (tapes[1].num_runs <= fib_1) {

        // }
    }

    disk_close_file(&input);

    return 0;
}

void init_tapes() {
    for (int i = 0; TAPES_NUM < 3; i++) {
        tapes[i].num_runs = 0;
        tapes[i].num_records = 0;
    }
    tapes[0].file_path = TAPE_0_DIR;
    tapes[1].file_path = TAPE_1_DIR;
    tapes[2].file_path = TAPE_2_DIR;
}
