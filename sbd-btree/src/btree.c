#include "btree.h"
#include "file.h"
#include "disk.h"

int btree_init(struct file *file, struct record *initial_record, int tree_order) {
    int status = 0;
    struct btree *tree = &file->btree;

    tree->order = tree_order;
    tree->height = 1;

    status = page_init(&tree->root, tree->order, 1, -1, 0);
    page_insert_record(file, &tree->root, initial_record, tree->order);

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
int btree_search(struct file *file, int key, int *page_index, int *element_index) {
    struct btree *tree = &file->btree;

    btree_get_page(file, 0, tree->root);
    int status = btree_search_page(file, key, tree->root, element_index);

    *page_index = file->current_page.page_index;

    if (status < 0) {
        return -1;
    }

    return 0;
}

/// @brief Split an overflown child of parent_page by index
/// @return 
int btree_split_child_page(struct file *file, struct page *parent_page, int child_index) {
    int status = 0;
    struct btree *tree = &file->btree;

    struct page page_to_split;
    btree_get_page(file, parent_page->page_pointers[child_index], &page_to_split);

    struct page *new_page;
    page_init(&new_page, tree->order, 0, parent_page->page_index, disk_next_page_index());

    new_page->number_of_elements = tree->order - 1;
    for (int i = 0; i < tree->order - 1; i++) {
        new_page->keys[i] = page_to_split.keys[i + tree->order];
        new_page->data_pointers[i] = page_to_split.data_pointers[i + tree->order];
    }

    if (!page_is_leaf(&page_to_split)) {
        for (int i = 0; i < tree->order; i++) {
            new_page->page_pointers[i] = page_to_split.page_pointers[i + 1];
        }
    }

    page_to_split.number_of_elements = tree->order - 1;

    for (int i = parent_page->number_of_elements; i >= child_index; i--) {
        parent_page->page_pointers[i + 1] = parent_page->page_pointers[i];
    }
    for (int i = parent_page->number_of_elements - 1; i >= child_index; i--) {
        parent_page->keys[i + 1] = parent_page->keys[i];
        parent_page->data_pointers[i + 1] = parent_page->keys[i];
    }

    parent_page->keys[child_index] = page_to_split.keys[tree->order - 1];
    parent_page->page_pointers[child_index + 1] = new_page->page_index;

    parent_page->number_of_elements += 1;

    btree_set_page(file, parent_page->page_index, parent_page);
    btree_set_page(file, page_to_split.page_index, &page_to_split);
    btree_set_page(file, new_page->page_index, new_page);
    
    return status;
}

int btree_insert_record(struct file *file, struct record *record) {
    int status = 0;
    struct btree *tree = &file->btree;

    int page_index;
    int element_index;
    status = btree_search(file, record->id, &page_index, &element_index);
    if (status < 0) {
        printf("%s: key %ld already exists\n", __func__, record->id);
        return -1;
    }

    printf("key %ld should be at page %d at index %d\n", record->id, page_index, element_index);

    struct page *current = &file->current_page;
    if (file->current_page.number_of_elements < 2 * tree->order) {
        page_insert_record(file, &current, record, element_index);

        btree_set_page(file, file->current_page.page_index, &file->current_page);

        return 0;
    }

    // OVERFLOW !

    // try compensation

    // compensation not possible

    //page_insert_record(file, &current, record, element_index);

    // if root is being split, make it a child of a new empty node
    if (file->current_page.is_root) {
        struct page *child_page;

        page_init(&child_page, tree->order, 1, -1, disk_next_page_index());
        file->current_page.is_root = 0;
        file->current_page.parent_page_pointer = child_page->page_index;
        tree->root = child_page;
        child_page->page_pointers[0] = file->current_page.page_index;
    }
    //btree_split_child_page(file, &file->current_page, 0); // 0 because root is split

    return 0;
}
