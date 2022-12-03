#include "record.h"
#include "file.h"
#include "common/status.h"
#include <stdio.h>
#include "disk.h"
#include "sort.h"

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

int main () {

    int requested_records = 1000000;

    //disk_generate_random("data/generated.file", requested_records);
    disk_generate_records("data/generated.file", requested_records);

    sort_distribution_phase("data/generated.file");

    sort_sorting_phase();

    sort_postprocess_phase(requested_records, 0);



    // // TODO remove debug
    // long hashdupy = hashchuj("AAA", 73);

    // printf("\n test hash %ld\n", hashdupy);
    // for (int i = 0; i < 3; i++) {
    //     char letter;

    //     letter = (char) 'A' + (hashdupy % 25);
    //     hashdupy /= 25;

    //     printf("%c\n", letter);
    // }

    // hashdupy = hashchuj("AAC", 73);

    // printf("\n test hash %ld\n", hashdupy);
    // for (int i = 0; i < 3; i++) {
    //     char letter;

    //     letter = (char) 'A' + (hashdupy % 25);
    //     hashdupy /= 25;

    //     printf("%c\n", letter);
    // }

    // struct tape input;
    // input.path = "data/generated2.file";
    // disk_open_file(&input, "rb+");
    // disk_debug_tape(&input);
    // disk_close_file(&input);

    return 0;
}
