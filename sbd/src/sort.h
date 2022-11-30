#ifndef SORT_H
#define SORT_H

#include "tape.h"
#include "common/config.h"

int sort_distribution_phase(const char *input_file_path);
int sort_sorting_phase();
void sort_postprocess_phase(int initial_elements_num, int print_contents);

#endif // SORT_H
