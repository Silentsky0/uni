#include "btree.h"
#include "disk.h"
#include "file.h"
#include "page.h"

void btree_get_page(struct file *file, int index, struct page *page);
void btree_set_page(struct file *file, int index, struct page *page);

int btree_init(struct file *file, const char *path, const char *data_path, int tree_order) {
    int status = 0;

    status = disk_open_file(file, path, data_path, "wb+");
    if (status < 0) {
        printf("%s: can't open file\n", __func__);
    }

    struct btree *tree = &file->btree;

    tree->order = tree_order;
    tree->height = 1;
    tree->num_pages = 1;

    status = page_init(&tree->root, tree->order, 1, -1, 0, 0);

    struct record tmp = tmp_record();
    page_insert_record(file, &tree->root, &tmp, 0);

    disk_write_page(file, &tree->root, 0);
    disk_update_tree_metadata(file);

    //disk_debug_page(file, 0);

    disk_close_file(file);

    return status;
}

int btree_open(struct file *file, const char *path, const char *data_path) {
    int status = 0;

    file->path = path;
    file->data_path = data_path;
    file->mode = "rb+";

    status = disk_open_file(file, path, data_path, "rb+");
    if (status < 0) {
        printf("%s: can't open file\n", __func__);
    }

    int root_index;
    disk_load_tree_metadata(file, &root_index);

    printf("tree order %d height %d root %d pages %d\n", file->btree.order, file->btree.height, root_index, file->btree.num_pages);

    disk_read_page(file, &file->btree.root, root_index);

    printf("root key %ld\n", file->btree.root.keys[0]);

    return 0;
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

    btree_get_page(file, 0, &tree->root);
    int status = btree_search_page(file, key, &tree->root, element_index);

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

    struct page new_page;
    page_init(&new_page, tree->order, 0, parent_page->page_index, disk_next_page_index(), parent_page->page_depth + 1);


    printf("\n=== split child index %d ===\n\n", child_index);

    printf("\nbefore split:\n");
    page_print(parent_page);
    page_print(&page_to_split);
    page_print(&new_page);

    // move elements on the right to a new page
    new_page.number_of_elements = tree->order;
    for (int i = 0; i < tree->order; i++) {
        new_page.keys[i] = page_to_split.keys[i + tree->order + 1];
        new_page.data_pointers[i] = page_to_split.data_pointers[i + tree->order + 1];
    }

    if (!page_is_leaf(&page_to_split)) {
        printf("page %ld is not a leaf\n", page_to_split.page_index);
        for (int i = 0; i < tree->order; i++) {
            new_page.page_pointers[i] = page_to_split.page_pointers[i + 1];
        }
    }

    page_to_split.number_of_elements = tree->order;

    for (int i = parent_page->number_of_elements; i >= child_index; i--) {
        parent_page->page_pointers[i + 1] = parent_page->page_pointers[i];
    }
    for (int i = parent_page->number_of_elements - 1; i >= child_index; i--) {
        parent_page->keys[i + 1] = parent_page->keys[i];
        parent_page->data_pointers[i + 1] = parent_page->keys[i];
    }

    // move the middle node of split node to parent
    parent_page->keys[child_index] = page_to_split.keys[tree->order];
    parent_page->data_pointers[child_index] = page_to_split.data_pointers[tree->order];

    parent_page->page_pointers[child_index + 1] = new_page.page_index;
    parent_page->number_of_elements += 1;

    printf("\nafter split\n");
    page_print(parent_page);
    page_print(&page_to_split);
    page_print(&new_page);

    btree_set_page(file, parent_page->page_index, parent_page);
    btree_set_page(file, page_to_split.page_index, &page_to_split);
    btree_set_page(file, new_page.page_index, &new_page);
    
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

    if (file->current_page.number_of_elements < 2 * tree->order) {
        page_insert_record(file, &file->current_page, record, element_index);

        btree_set_page(file, file->current_page.page_index, &file->current_page);

        return 0;
    }

    // OVERFLOW !

    // try compensation

    // compensation not possible

    // if root is being split, make it a child of a new empty node
    if (file->current_page.is_root) {
        struct page child_page;

        page_insert_record(file, &file->current_page, record, element_index);

        page_init(&child_page, tree->order, 1, -1, disk_next_page_index(), 0);
        file->current_page.is_root = 0;
        file->current_page.parent_page_pointer = child_page.page_index;
        tree->root = child_page;
        btree_set_page(file, file->current_page.page_index, &file->current_page);
        child_page.page_pointers[0] = file->current_page.page_index;

        btree_split_child_page(file, &child_page, 0); // 0 because root is split
    }
   

    return 0;
}
