#include "buffer.h"
#include <stdlib.h>
#include "disk.h"

/*
 * Sets default values for a buffer
 */
void buffer_init(struct buffer *b) {
    b->block = malloc(sizeof(struct block));
    b->record_index = 0;
    b->block_index = 0;
    b->last_accessed_file = NULL;
}

void buffer_close(struct buffer *b) {
    free(b->block);
}

void buffer_dump(struct buffer *b) {
    
}
