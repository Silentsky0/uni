#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>

struct buffer {
    struct page *buffer;
    int pages;
};

void buffer_init(struct buffer *b);

#endif // BUFFER_H
