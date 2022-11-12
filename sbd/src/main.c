#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"

struct record test_record(void);

int main () {

    disk_generate_random("data/generated.file", 10);

    disk_print_file("data/generated.file");

    return 0;
}

struct record test_record(void) {
    struct record test;

    test.data.age = 3;
    //test.data.name = "Pawel";
    //test.data.surname = "test";

    //test.id.identity_series = "DAM";
    test.id.identity_number = 12345;

    return test;
}
