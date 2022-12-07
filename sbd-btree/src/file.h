#ifndef FILE_H
#define FILE_H

#include "btree.h"
#include <stdio.h>
#include "buffer.h"


struct file {
    const char *path;
    FILE *file;
    const char *mode;

    struct buffer buffer;

    struct btree btree;
};

#endif // FILE_H
