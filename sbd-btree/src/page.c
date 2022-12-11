#include "page.h"
#include "common/status.h"
#include "memory.h"
#include <stdlib.h>
#include "btree.h"
#include "disk.h"
#include <stdio.h>
#include "file.h"

int page_init(struct page *page, int tree_order, int is_root, long parent_page, int page_index, int page_depth) {

    int status = 0;

    if (is_root) {
        //*page = (struct page *)malloc(sizeof(struct page));
        page->number_of_elements = 0;
        page->is_root = 1;
        page->parent_page_pointer = -1;
        page->page_index = page_index;
        page->page_depth = 0;

        page->page_pointers[0] = -1;
        page->page_pointers[1] = -1;

        return 0;
    }

    //page = (struct page *)malloc(sizeof(struct page));
    page->number_of_elements = 0;
    page->is_root = 0;
    page->parent_page_pointer = parent_page;
    page->page_index = page_index;
    page->page_depth = page_depth;

    page->page_pointers[0] = -1;
    for (int i = 0; i < tree_order; i++) {
        page->keys[i] = 0;
        page->data_pointers[i] = -1;
        page->page_pointers[i + 1] = -1;
    }

    return status;
}

/// @brief Insert recordd at selected index in page (where it should be)
/// @return Status code
int page_insert_record(struct file *file, struct page *page, struct record *record, int index) {

    int number_of_elements = page->number_of_elements;

    for (int i = number_of_elements; i > index; i--) {
        page->keys[i] = page->keys[i - 1];
        page->data_pointers[i] = page->data_pointers[i - 1];
        page->page_pointers[i + 1] = page->page_pointers[i];
    }

    page->keys[index] = record->id;
    page->page_pointers[index + 1] = -1;

    disk_add_record(file, page, record, index);

    page->number_of_elements += 1;

    return 0;
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

int page_is_leaf(struct page *page) {
    for (int i = 0; i <= page->number_of_elements; i++) {
        if (page->page_pointers[i] != -1)
            return 0;
    }
    return 1;
}
