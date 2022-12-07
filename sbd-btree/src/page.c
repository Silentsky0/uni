#include "page.h"
#include "common/status.h"
#include "memory.h"
#include <stdlib.h>
#include "btree.h"

#include <stdio.h>

int page_init(struct page **page, int tree_order) {

    int status = 0;

    *page = (struct page *)malloc(sizeof(struct page));
    (*page)->number_of_elements = tree_order * 2;

    return status;
}
