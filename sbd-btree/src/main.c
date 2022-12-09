#include <stdio.h>

#include "disk.h"
#include "file.h"
#include "btree.h"

int main() {

    struct file test;

    // init
    disk_init_file(&test, "data/test.file", 2);


    // check if it is working
    disk_open_file(&test, "data/test.file", "rb+");
    disk_debug_page(&test, 0);
    disk_close_file(&test);

    return 0;
}
