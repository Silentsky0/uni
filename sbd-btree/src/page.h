#ifndef PAGE_H
#define PAGE_H

struct btree;
struct file;

#include "common/config.h"
#include "record.h"

struct page {
    // header
    long page_index;
    int number_of_elements;
    int is_root;
    long parent_page_pointer;
    int page_depth;

    // data
    long keys[MAX_TREE_ORDER * 2];
    long data_pointers[MAX_TREE_ORDER * 2];
    long page_pointers[MAX_TREE_ORDER * 2 + 1];
} __attribute__((__packed__));

int page_init(struct page *page, int tree_order, int is_root, long parent_page, int page_index, int page_depth);

int page_insert_record(struct file *file, struct page *page, struct record *record, int index);

int page_search_bisection(struct page *page, int key, int left, int right);

int page_is_leaf(struct page *page);
void page_print(struct page *page);

#endif // PAGE_H
