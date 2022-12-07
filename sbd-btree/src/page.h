#ifndef PAGE_H
#define PAGE_H

struct btree;
struct page;

#include "common/config.h"
#include "btree.h"

struct page {
    key keys[MAX_TREE_ORDER * 2];
    long data_pointers[MAX_TREE_ORDER * 2];
    long page_pointers[MAX_TREE_ORDER * 2 + 1];

    int number_of_elements;
} __attribute__((__packed__));

int page_init(struct page **page, int tree_order);

#endif // PAGE_H
