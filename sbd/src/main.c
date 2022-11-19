#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"
#include "sort.h"

int main () {

    disk_generate_random("data/generated.file", 20);

    sort_distribution_phase("data/generated.file");


    // TEST
    sort_single_phase(0, 1, 2);

    return 0;
}
