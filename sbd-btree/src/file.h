#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "buffer.h"

#include "btree.h"

struct file {
    const char *path;
    FILE *file;
    const char *mode;

    const char *data_path;
    FILE *data_file;

    struct page current_page;

    struct btree btree;
};

#endif // FILE_H
