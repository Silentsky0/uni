#ifndef BTREE_H
#define BTREE_H

struct file;

#include "page.h"
#include "record.h"

struct btree {

    int order;
    int height;

    int num_pages;

    struct page *root;
};

int btree_init(struct file *file, struct record *initial_record, int tree_order);

#endif // BTREE_H
