#ifndef CONFIG_H
#define CONFIG_H

#include "../record.h"

#define BLOCK_SIZE 512
#define RECORDS_IN_BLOCK (BLOCK_SIZE / sizeof(struct record))

#define MAX_RANDOM_IDS 1048576 // 2^20

#define TAPES_NUM 3

#define TAPE_0_DIR "tapes/t0.tape"
#define TAPE_1_DIR "tapes/t1.tape"
#define TAPE_2_DIR "tapes/t2.tape"

#endif // CONFIG_H
