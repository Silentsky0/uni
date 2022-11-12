#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"

int main () {

    disk_generate_random("data/generated.file", 30);

    disk_print_file("data/generated.file");

    return 0;
}
