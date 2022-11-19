#ifndef SORT_H
#define SORT_H

#include "tape.h"
#include "common/config.h"

int sort_distribution_phase(const char *input_file_path);
void sort_single_phase(int tape_1, int tape_2, int merge_tape);

#endif // SORT_H
