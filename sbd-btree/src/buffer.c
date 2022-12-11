#include "buffer.h"
#include <stdlib.h>
#include "disk.h"
#include <memory.h>

/*
 * Sets default values for a buffer
 */
void buffer_init(struct buffer *b) {
    b->buffer = malloc(sizeof(struct page));
    b->pages = 1;
}

void buffer_add_page(struct file *file, struct buffer *b, struct page *page, int depth) {
    
}

void buffer_get_page(struct file *file, struct buffer *b, struct page *page, int depth) {
    
}

// void buffer_clear(struct buffer *b) {
//     //memset(b->block, 0, sizeof(struct block));
//     b->block_index = 0;
//     b->record_index = 0;
//     b->buffer_records = 0;
// }

// void buffer_close(struct buffer *b) {
//     b->buffer_records = 0;
//     free(b->block);
//     b->block = NULL;
// }
