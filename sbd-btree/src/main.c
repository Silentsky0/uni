#include <stdio.h>

#include "disk.h"
#include "file.h"
#include "btree.h"

int main() {

    struct file test;

    // init
    btree_init(&test, "data/tree.file", "data/data.file", 2);


    // check if it is working
    btree_open(&test, "data/tree.file", "data/data.file");

    //disk_debug_page(&test, 0);
    //printf("\n");

    struct record t = tmp_record_by_key(50);
    btree_insert_record(&test, &t);

    t = tmp_record_by_key(14);
    btree_insert_record(&test, &t);

    t = tmp_record_by_key(9);
    btree_insert_record(&test, &t);
    disk_debug_page(&test, 0);


    t = tmp_record_by_key(15);
    btree_insert_record(&test, &t);

    t = tmp_record_by_key(3);
    btree_insert_record(&test, &t);


    printf("\n==== END ====\n\n");
    disk_debug_page(&test, 0);
    disk_debug_page(&test, 1);
    disk_debug_page(&test, 2);
    disk_close_file(&test);

    return 0;
}
