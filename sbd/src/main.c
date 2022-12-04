#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"
#include "sort.h"
#include <string.h>
#include "stdlib.h"

int hashchuj(const char* str, int h)
{
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

int main (int argc, char *argv[]) {

    int requested_records = 10;

    int print_every_phase = 0;
    int print_contents = 0;

    if (argc >= 2) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                printf("");

                return 0;
            }

            if (strcmp(argv[i], "-n") == 0 && argc >= i + 1) {
                requested_records = atoi(argv[++i]);
            }

            if (strcmp(argv[i], "--keyboard") == 0 || strcmp(argv[i], "-k") == 0) {
                // TODO
            }

            if (strcmp(argv[i], "--print-contents") == 0 || strcmp(argv[i], "-c") == 0) {
                print_contents = 1;
            }

            if (strcmp(argv[i], "--print-every-phase") == 0 || strcmp(argv[i], "-p") == 0) {
                print_every_phase = 1;
            }

            if (strcmp(argv[i], "--file-path") == 0 || strcmp(argv[i], "-f") == 0) {

            }

        }
    }



    //disk_generate_random("data/generated.file", requested_records);
    disk_generate_records("data/generated.file", requested_records);

    sort_distribution_phase("data/generated.file", print_contents);

    sort_sorting_phase(print_contents, print_every_phase);

    sort_postprocess_phase(requested_records, 0);

    return 0;
}
