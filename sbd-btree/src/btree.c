#include "btree.h"
#include "file.h"
#include "disk.h"

int btree_init(struct file *file, struct record *initial_record, int tree_order) {
    int status = 0;
    struct btree *tree = &file->btree;

    tree->order = tree_order;
    tree->height = 1;

    status = page_init(&tree->root, tree->order, 1, -1);

    tree->root->keys[0] = initial_record->id;

    return status;
}

/// @brief Load a page into memory by index
/// @return 0 on success, < 0 otherwise
void btree_get_page(struct file *file, int index, struct page *page) {

    int status = disk_read_page(file, page, index);
    if (status < 0) {
        printf("%s: error getting page at index %d\n", __func__, index);
        page = NULL;
    }
}

void btree_set_page(struct file *file, int index, struct page *page) {
    int status = disk_write_page(file, page, index);
    if (status < 0) {
        printf("%s: error setting page at index %d\n", __func__, index);
    }
}

/// @brief 
/// @return 
int btree_search_page(struct file *file, int key, struct page *page, int *index) {

    int element_index = page_search_bisection(page, key, 0, page->number_of_elements - 1);

    *index = element_index;

    // element was found
    if (page->keys[element_index] == key) {
        return -1;
    }

    // element was not found
    if (page->page_pointers[element_index] == -1) {
        return 0;
    }

    struct page next_page;
    btree_get_page(file, page->page_pointers[element_index], &next_page);
    return btree_search_page(file, key, &next_page, index);
}

/// @brief Search for page which contains record by key
/// @return returns 0 if element was not found, -1 if it was
int btree_search(struct file *file, int key, int *page) {
    struct btree *tree = &file->btree;

    file->current_page = *(tree->root);
    int status = btree_search_page(file, key, tree->root, page);

    if (status < 0) {
        return -1;
    }

    return 0;
}

int btree_insert_record(struct file *file, struct record *record) {
    int status = 0;
    struct btree *tree = &file->btree;

    int *page_index;
    status = btree_search(file, record->id, page_index);
    if (status < 0) {
        printf("%s: key %ld already exists\n", __func__, record->id);
        return -1;
    }

    printf("test %d\n", *page_index);

    struct page *current = &file->current_page;

    if (file->current_page.number_of_elements < 2 * tree->order) {
        page_insert_record(&current, record);
    }

    btree_set_page(file, file->current_page.page_index, &file->current_page);

    printf("tsdsd %d\n", current->number_of_elements);

    return 0;
}
