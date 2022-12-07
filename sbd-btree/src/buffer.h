#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>

struct buffer {
    struct block *block;
    int record_index;
    int block_index;
    int buffer_records;
};

void buffer_init(struct buffer *b);
void buffer_close(struct buffer *b);
void buffer_clear(struct buffer *b);

#endif // BUFFER_H
