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
int disk_open_file(struct file *file) {
    
    file->file = fopen(file->path, file->mode);
    if (file->file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, file->path);
        return -EIO;
    }

    buffer_init(&file->buffer);

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

    status = fread((void *) &page->page_pointers[0], sizeof(page->page_pointers[0]), 1, file->file);
    if (status < 1) {
        printf("%s: fread error\n", __func__);
        return -EIO;
    }
    for (int i = 0; i < 3 * 2 * file->btree.order; i++) {
        status = fread((void *) &page->keys[i], sizeof(page->keys[0]), 1, file->file);
        status = fread((void *) &page->data_pointers[i], sizeof(page->data_pointers[0]), 1, file->file);
        status = fread((void *) &page->page_pointers[i + 1], sizeof(page->page_pointers[0]), 1, file->file);
    }
    if (status < 3 * 2 * file->btree.order) {
        printf("%s: fread error\n", __func__);
        return -EIO;
    }

    return 0;
}
