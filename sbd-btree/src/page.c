#include "page.h"
#include "common/status.h"
#include "memory.h"
#include <stdlib.h>
#include "btree.h"

#include <stdio.h>

int page_init(struct page **page, int tree_order, int is_root, long parent_page) {

    int status = 0;

    if (is_root) {
        *page = (struct page *)malloc(sizeof(struct page));
        (*page)->number_of_elements = 1;
        (*page)->is_root = 1;
        (*page)->parent_page_pointer = -1;
        (*page)->page_index = 0;

        (*page)->data_pointers[0] = 0; // TODO point to some data
        (*page)->page_pointers[0] = -1;
        (*page)->page_pointers[1] = -1;

        return 0;
    }

    *page = (struct page *)malloc(sizeof(struct page));
    (*page)->number_of_elements = 0;
    (*page)->is_root = 0;
    (*page)->parent_page_pointer = parent_page;
    
    (*page)->page_pointers[0] = -1;
    for (int i = 0; i < tree_order; i++) {
        (*page)->keys[i] = 0;
        (*page)->data_pointers[i] = 0;
        (*page)->page_pointers[i + 1] = -1;
    }

    return status;
}

/// @brief 
/// @return index where the key was found (or where it should be)
int page_search_bisection(struct page *page, int key, int left, int right) {
    if (left > right) {
        return left;
    }

    int pivot = (left + right) / 2;

    if (key < page->keys[pivot]) {
        return page_search_bisection(page, key, left, pivot - 1);
    }
    if (key > page->keys[pivot]) {
        return page_search_bisection(page, key, pivot + 1, right);
    }

    return pivot;
}
