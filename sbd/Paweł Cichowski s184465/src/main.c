#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"
#include "sort.h"
#include <string.h>
#include "stdlib.h"



int main (int argc, char *argv[]) {

    int requested_records = 10;

    int print_every_phase = 0;
    int print_contents = 0;
    int used_keyboard = 0;

    char *data_path = "data/generated.file";

    if (argc >= 2) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                
                printf("\nPolyphase merge sort\n");
                printf("Paweł Cichowski 2022\n\n");
                printf("  Available commands:\n");
                printf("    -h, --help: displays this message\n");
                printf("    -n, --n-to-generate: sets how many records to randomly generate\n");
                printf("    -k, --keyboard: allows for keyboard input\n");
                printf("    -c, --print-contents: displays the file contents before and after sorting\n");
                printf("    -p, --print-every-phase: displays tape contents after each sorting phase\n");
                printf("    -f, --file-path: specifies a path from which records will be retrieved\n");
                printf("\n");

                return 0;
            }

            if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--n-to-generate") == 0) && argc >= i + 1) {
                requested_records = atoi(argv[++i]);
            }

            if (strcmp(argv[i], "--keyboard") == 0 || strcmp(argv[i], "-k") == 0) {
                requested_records = disk_keyboard_input("data/keyboard-input.file");
                used_keyboard = 1;
            }

            if (strcmp(argv[i], "--print-contents") == 0 || strcmp(argv[i], "-c") == 0) {
                print_contents = 1;
            }

            if (strcmp(argv[i], "--print-every-phase") == 0 || strcmp(argv[i], "-p") == 0) {
                print_every_phase = 1;
            }

            if (strcmp(argv[i], "--file-path") == 0 || strcmp(argv[i], "-f") == 0) {
                data_path = argv[++i];
            }
        }
    }

    if (!used_keyboard)
        disk_generate_records(data_path, requested_records);

    sort_distribution_phase(data_path, print_contents);

    sort_sorting_phase(print_contents, print_every_phase);

    sort_postprocess_phase(requested_records, 0, used_keyboard);
}


