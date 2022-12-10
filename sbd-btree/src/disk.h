#ifndef DISK_H
#define DISK_H

#include "common/config.h"
#include "file.h"
#include "page.h"

int disk_init_file(struct file *file, const char *path, const char *data_path, int tree_order);

int disk_open_file(struct file *file, const char *path, const char *data_path, const char *mode);
int disk_close_file(struct file *file);

int disk_write_page(struct file *file, struct page *page, int index);
int disk_read_page(struct file *file, struct page *page, int index);

int disk_add_record(struct file *file, struct page *page, struct record *record, int index);

int disk_next_page_index();
int disk_next_record_index();

// debug
void disk_debug_page(struct file *file, int index);

#endif // DISK_H
