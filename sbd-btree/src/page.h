#ifndef PAGE_H
#define PAGE_H

struct btree;
struct page;

#include "common/config.h"
#include "btree.h"
#include "record.h"

struct page {
    // header
    long page_index;
    int number_of_elements;
    int is_root;
    long parent_page_pointer;

    // data
    key keys[MAX_TREE_ORDER * 2];
    long data_pointers[MAX_TREE_ORDER * 2];
    long page_pointers[MAX_TREE_ORDER * 2 + 1];
} __attribute__((__packed__));

int page_init(struct page **page, int tree_order, int is_root, long parent_page);

int page_insert_record(struct page **page, struct record *record);

int page_search_bisection(struct page *page, int key, int left, int right);

#endif // PAGE_H
