#include <stdio.h>

#include "disk.h"
#include "file.h"


int main() {

    struct file test;
    test.path = "data/test.file";
    test.mode = "wb+";

    test.btree.order = 2;
    test.btree.height = 2;

    disk_open_file(&test);

    page_init(&test.btree.root, test.btree.order);

    disk_write_page(&test, test.btree.root, 0);

    disk_close_file(&test);

    return 0;
}
