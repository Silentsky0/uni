#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"

struct record test_record(void);

int main () {

    struct file file;

    file_init(&file);

    //int status = file_import(&file, "./data/5-people.file");

    //if (status < 0)
    //    printf("error importing records from file, error code %d", status);

    //random_record(NULL);

    disk_generate_random("data/generated.file", 10);

    //file_print(&file);

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
