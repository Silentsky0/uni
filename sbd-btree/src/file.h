#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "buffer.h"

#include "btree.h"

struct file {
    const char *path;
    FILE *file;
    const char *mode;

    struct buffer buffer;

    struct btree btree;
};

#endif // FILE_H
