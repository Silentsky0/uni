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

        return 0;
    }

    *page = (struct page *)malloc(sizeof(struct page));
    (*page)->number_of_elements = tree_order * 2;
    (*page)->is_root = 0;
    (*page)->parent_page_pointer = parent_page;

    return status;
}

/// @brief 
/// @return index where the key was found (or where it should be)
int page_search_bisection(struct page *page, int key, int left, int right) {
    if (left > right) {
        if (right == -1)
            return -1;
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
