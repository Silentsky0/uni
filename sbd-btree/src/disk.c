#include "disk.h"
#include "common/status.h"
#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "page.h"


/*
 * Helper functions
 */

int tree_page_size(struct btree *btree) {
    return btree->order * sizeof(long) * 4 +   // keys and data pointers
           (btree->order * 2 + 1) * sizeof(long);    // pointers to child pages
}

int tree_real_page_size(struct btree *btree) {
    return SECTOR_SIZE * (tree_page_size(btree) / SECTOR_SIZE + 1);
}


/*
 *  Open a file from disk
 *  assumes the path and mode are set
 */
int disk_open_file(struct file *file, const char *path, const char *mode) {
    
    file->file = fopen(path, mode);
    if (file->file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, file->path);
        return -EIO;
    }
    file->path = path;
    file->mode = mode;

    return 0;
}

int disk_close_file(struct file *file) {
    if (file->mode[0] == 'r') {
        fclose(file->file);
        return 0;
    }

    //struct buffer *buffer = &file->buffer;

    // TODO close buffer properly

    return 0;
}


/*
 * Writes a btree page to disk
 * assumes the file has been succesfully opened
 */
int disk_write_page(struct file *file, struct page *page, int index) {
    int page_size = tree_real_page_size(&file->btree);

    int status = fseek(file->file, index * page_size, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    key key = 0;
    long page_pointer = 0;
    long data_pointer = 0;

    // write page

    fwrite(&page->number_of_elements, sizeof(page->number_of_elements), 1, file->file); // TODO status
    fwrite(&page->is_root, sizeof(page->is_root), 1, file->file); // TODO status
    fwrite(&page->parent_page_pointer, sizeof(page->parent_page_pointer), 1, file->file);

    page_pointer = page->page_pointers[0];
    status = fwrite(&page_pointer, sizeof(page_pointer), 1, file->file);
    if (status < 1) {
        perror("Error: ");
        printf("%s: fwrite error %d\n", __func__, ferror(file->file));
        return -EIO;
    }

    for (int i = 0; i < page->number_of_elements; ++i) {
        key = page->keys[i];
        data_pointer = page->data_pointers[i];
        page_pointer = page->page_pointers[i + 1];

        status += fwrite((void *) &key, sizeof(key), 1, file->file);
        status += fwrite((void *) &data_pointer, sizeof(data_pointer), 1, file->file);
        status += fwrite((void *) &page_pointer, sizeof(page_pointer), 1, file->file);
    }
    if (status < 3 * page->number_of_elements) {
        perror("Error: ");
        printf("%s: fwrite error %d\n", __func__, ferror(file->file));
        return -EIO;
    }

    // write padding
    int padding_bytes = page_size - tree_page_size(&file->btree);
    int tmp = 0;
    for (int i = 0; i < padding_bytes; ++i) {
        status += fwrite(&tmp, sizeof(char), 1, file->file);
    }
    if (status < padding_bytes) {
        perror("Error: ");
        printf("%s: fwrite error %d\n", __func__, ferror(file->file));
        return -EIO;
    }

    return 0;
}


/*
 * Reads a btree page from disk
 * assumes the file has been opened succesfully 
 */
int disk_read_page(struct file *file, struct page *page, int index) {
    int page_size = tree_real_page_size(&file->btree);

    int status = fseek(file->file, index * page_size, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    fread(&page->number_of_elements, sizeof(page->number_of_elements), 1, file->file);
    fread(&page->is_root, sizeof(page->is_root), 1, file->file); // TODO status
    fread(&page->parent_page_pointer, sizeof(page->parent_page_pointer), 1, file->file);

    status = fread((void *) &page->page_pointers[0], sizeof(page->page_pointers[0]), 1, file->file);
    if (status < 1) {
        perror("Error: ");
        printf("%s: fread error %d\n", __func__, ferror(file->file));
        return -EIO;
    }
    for (int i = 0; i < page->number_of_elements; i++) {
        status = fread((void *) &page->keys[i], sizeof(page->keys[0]), 1, file->file);
        status = fread((void *) &page->data_pointers[i], sizeof(page->data_pointers[0]), 1, file->file);
        status = fread((void *) &page->page_pointers[i + 1], sizeof(page->page_pointers[0]), 1, file->file);
    }
    if (status < 1) {
        perror("Error: ");
        printf("%s: fread error %d\n", __func__, ferror(file->file));
        return -EIO;
    }

    file->current_page = *page;

    return 0;
}

void disk_debug_page(struct file *file, int index) {

    struct page page;

    int status = disk_read_page(file, &page, index);
    if (status < 0) {
        printf("%s: some error\n", __func__);
        return;
    }

    if (page.parent_page_pointer == -1) {
        printf("-- root page num of elements %d --\n", page.number_of_elements);
    }
    else {
        printf("-- page %d num of elements %d parent page %ld --\n", index, page.number_of_elements, page.parent_page_pointer);
    }

    printf("keys:\n  ");
    for (int i = 0; i < page.number_of_elements; i++) {
        printf("%ld ", page.keys[i]);
    }

    printf("\ndata pointers:\n  ");
    for (int i = 0; i < page.number_of_elements; i++) {
        printf("%ld ", page.data_pointers[i]);
    }

    printf("\npage pointers:\n  ");
    for (int i = 0; i < page.number_of_elements + 1; i++) {
        printf("%ld ", page.page_pointers[i]);
    }
    printf("\n");

}

/// @brief Initialises a file with the B-Tree structure
/// @return status code
int disk_init_file(struct file *file, const char *path, int tree_order) {

    disk_open_file(file, path, "rb+");

    struct record initial = tmp_record();
    btree_init(file, &initial, tree_order);

    disk_write_page(file, file->btree.root, 0);

    disk_close_file(file);

    return 0; // TODO add status
}
