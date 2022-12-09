#include "btree.h"
#include "file.h"

int btree_init(struct file *file, struct record *initial_record, int tree_order) {
    int status = 0;
    struct btree *tree = &file->btree;

    tree->order = tree_order;
    tree->height = 1;

    status = page_init(&tree->root, tree->order, 1, -1);

    tree->root->keys[0] = initial_record->id;

    return status;
}

/// @brief Search for page which contains record by key
/// @return returns 0 on success, < 0 otherwise
int btree_search_record(struct file *file, int key, int *page) {
    int status = 0;
    struct btree *tree = &file->btree;

    long root_pointer = tree->root->page_index;

    return 0;
}

int btree_insert_record(struct file *file, struct record *record) {
    int status = 0;
    struct btree *tree = &file->btree;

    // TODO search for record in tree

    

    return 0;
}
