#ifndef CONFIG_H
#define CONFIG_H

#include "../record.h"

#define BLOCK_SIZE 512
#define RECORDS_IN_BLOCK (BLOCK_SIZE / sizeof(struct record))

#define MAX_RANDOM_IDS 1024

#endif // CONFIG_H
