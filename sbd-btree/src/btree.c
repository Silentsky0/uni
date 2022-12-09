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

int btree_search_page(struct file *file, int key, struct page *page) {

    int index = page_search_bisection(page, key, 0, page->number_of_elements - 1);

    if (page->page_pointers[index] == 0) { // TODO initialise page pointers to -1
        return -1;
    }

    if (page->keys[index] == key) {
        return index;
    }

    struct page next_page;
    btree_get_page(file, page->page_pointers[index], &next_page);
    return btree_search_page(file, key, &next_page);

}

/// @brief Search for page which contains record by key
/// @return returns 0 on success, < 0 otherwise
int btree_search(struct file *file, int key, int *page) {
    struct btree *tree = &file->btree;

    int page_index = btree_search_page(file, key, tree->root);

    page = &page_index;

    if (page_index < 0)
        return -1;

    return 0;
}

int btree_insert_record(struct file *file, struct record *record) {
    int status = 0;
    struct btree *tree = &file->btree;

    // TODO search for record in tree

    

    return 0;
}
