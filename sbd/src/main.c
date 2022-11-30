#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"
#include "sort.h"

int main () {

    int requested_records = 2000;

    disk_generate_random("data/generated.file", requested_records);

    sort_distribution_phase("data/generated.file");

    sort_sorting_phase();

    sort_postprocess_phase(requested_records, 0);

     
    // test
    // struct tape tape;
    // tape.path = "tapes/t0.tape";
    // tape.file = fopen(tape.path, "rb+");
    // if (tape.file == NULL) {
    //     printf("%s: file %s doesn't exist\n", __func__, tape.path);
    //     return -EIO;
    // }

    // printf("---------\n\n");

    // disk_print_file(&tape);


    return 0;
}
