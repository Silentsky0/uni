#include "disk.h"
#include "common/status.h"
#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "page.h"

int metadata_offset = 4 * sizeof(int);

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

int tree_data_page_size(struct btree *btree) {
    return btree->order * 2 * sizeof(struct record);
}
/// aligned to sector
int tree_real_data_page_size(struct btree *btree) {
    return SECTOR_SIZE * (tree_data_page_size(btree) / SECTOR_SIZE + 1);
}

/*
 *  Open a file from disk
 *  assumes the path and mode are set
 */
int disk_open_file(struct file *file, const char *path, const char *data_path, const char *mode) {

    file->file = fopen(path, mode);
    if (file->file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, path);
        return -EIO;
    }
    file->data_file = fopen(data_path, mode);
    if (file->data_file == NULL) {
        printf("%s: file %s doesn't exist\n", __func__, data_path);
        return -EIO;
    }
    file->path = path;
    file->data_path = data_path;
    file->mode = mode;

    return 0;
}

int disk_close_file(struct file *file) {
    if (file->mode[0] == 'r') {
        fclose(file->file);
        fclose(file->data_file);
        return 0;
    }

    fclose(file->file);
    fclose(file->data_file);

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

    int status = fseek(file->file, index * page_size + metadata_offset, 0);
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
    fwrite(&page->page_depth, sizeof(page->page_depth), 1, file->file);

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

        printf("writing key %ld data %ld\n", page->keys[i], page->data_pointers[i]);
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

    int status = fseek(file->file, index * page_size + metadata_offset, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    fread(&page->number_of_elements, sizeof(page->number_of_elements), 1, file->file);
    fread(&page->is_root, sizeof(page->is_root), 1, file->file); // TODO status
    fread(&page->parent_page_pointer, sizeof(page->parent_page_pointer), 1, file->file);
    fread(&page->page_depth, sizeof(page->page_depth), 1, file->file);

    // printf("depth %d parent %ld\n", page->page_depth, page->parent_page_pointer);

    status = fread((void *) &page->page_pointers[0], sizeof(page->page_pointers[0]), 1, file->file);
    if (status < 1) {
        perror("Error: ");
        printf("%s: fread error %d status %d\n", __func__, ferror(file->file), status);
        return -EIO;
    }
    for (int i = 0; i < page->number_of_elements; i++) {
        status = fread((void *) &page->keys[i], sizeof(page->keys[0]), 1, file->file);
        status = fread((void *) &page->data_pointers[i], sizeof(page->data_pointers[0]), 1, file->file);
        status = fread((void *) &page->page_pointers[i + 1], sizeof(page->page_pointers[0]), 1, file->file);

        printf("reading key %ld data %ld\n", page->keys[i], page->data_pointers[i]);
    }
    if (status < 1) {
        perror("Error: ");
        printf("%s: fread error %d status %d\n", __func__, ferror(file->file), status);
        return -EIO;
    }

    file->current_page = *page;

    return 0;
}

int disk_update_tree_metadata(struct file *file) {
    int status = fseek(file->file, 0, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    struct btree *tree = &file->btree;

    fwrite((void *)&tree->order, sizeof(tree->order), 1, file->file);
    fwrite((void *)&tree->height, sizeof(tree->height), 1, file->file);
    fwrite((void *)&tree->num_pages, sizeof(tree->num_pages), 1, file->file);
    fwrite((void *)&tree->root.page_index, sizeof(int), 1, file->file);

    return 0;
}

int disk_load_tree_metadata(struct file *file, int *root_index) {
    int status = fseek(file->file, 0, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    struct btree *tree = &file->btree;

    fread(&tree->order, sizeof(tree->order), 1, file->file);
    fread(&tree->height, sizeof(tree->height), 1, file->file);
    fread(&tree->num_pages, sizeof(tree->num_pages), 1, file->file);
    fread(root_index, sizeof(int), 1, file->file);

    return 0;
}

/// @brief Add record to data file
/// @return 
int disk_add_record(struct file *file, struct page *page, struct record *record, int index) {
    int next_record_index = disk_next_record_index();

    int status = fseek(file->data_file, sizeof(struct record) * next_record_index, 0);
    if (status != 0) {
        printf("%s: fseek error\n", __func__);
        return -EIO;
    }

    status = fwrite(record, sizeof(struct record), 1, file->data_file);
    if (status < 1) {
        perror("Error: ");
        printf("%s: fread error %d\n", __func__, ferror(file->file));
        return -EIO;
    }

    page->data_pointers[index] = next_record_index;

    return status;
}

void disk_debug_page(struct file *file, int index) {

    struct page page;

    int status = disk_read_page(file, &page, index);
    if (status < 0) {
        printf("%s: error reading page %d\n", __func__, index);
        return;
    }

    if (page.parent_page_pointer == -1) {
        printf("-- root page num of elements %d --\n", page.number_of_elements);
    }
    else {
        printf("-- page %d num of elements %d parent page %ld depth %d--\n", index, page.number_of_elements, page.parent_page_pointer, page.page_depth);
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

// /// @brief Initialises a file with the B-Tree structure
// /// @return status code
// int disk_init_file(struct file *file, const char *path, const char *data_path, int tree_order) {
//     int status;

//     status = disk_open_file(file, path, data_path, "wb+");
//     if (status < 0) {
//         printf("%s: can't open file\n", __func__);
//     }

//     struct record initial = tmp_record();
//     btree_init(file, &initial, tree_order);

//     //disk_write_page(file, file->btree.root, 0);

//     disk_close_file(file);

//     return 0; // TODO add status
// }

int disk_next_record_index() {
    static int record_index = -1; // TODO maybe start from -1

    record_index += 1;
    return record_index;
}

int disk_next_page_index() {
    static int page_index = 0;

    page_index += 1;
    return page_index;
}
