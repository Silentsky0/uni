#include "record.h"
#include "file.h"
#include "common/status.h"
#include "stdio.h"

struct record test_record(void);

int main () {

    struct file file;

    file_init(&file);
    
    struct record t = test_record();

    file_append_record(&file, &t);
    
    printf("%s%d\n\n", test_record().id.identity_series, test_record().id.identity_number);

    int status = file_import("./data/5-people.file");

    if (status < 0)
        printf("there's something wrong, error code %d", status);

    return 0;
}

struct record test_record(void) {
    struct record test;

    test.data.age = 3;
    test.data.name = "Pawel";
    test.data.surname = "test";

    test.id.identity_series = "DAM";
    test.id.identity_number = 12345;

    return test;
}
