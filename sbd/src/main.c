#include "record.h"
#include "common/status.h"
#include "stdio.h"

int main () {

    printf("test 123\n");

    struct record test;

    test.data.age = 3;
    test.data.name = "Pawel";
    test.data.surname = "test";

    test.id.identity_series = "DAM";
    test.id.identity_number = 12345;
    
    printf("%s\n", record_to_string(&test));

    return 0;
}
