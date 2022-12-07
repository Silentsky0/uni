#ifndef BTREE_H
#define BTREE_H

#include "page.h"

struct btree {

    struct page *root;

    int order;
    int height;

};

#endif // BTREE_H
