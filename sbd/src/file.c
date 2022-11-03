#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "common/status.h"

void file_init(struct file* file) {
    file->num_records = 0;
    file->records = NULL;
    file->overflow = NULL;
}

int file_append_record(struct file *file, struct record* record) {
    
    // TODO simplified version

    file->overflow = malloc(5 * sizeof(struct record));

    if (file->overflow == NULL)
        return -ENOMEM;

    file->ov_start = file->overflow;

    memcpy(file->ov_start, (void *)record, sizeof(struct record));
    file->ov_start += sizeof(struct record);

    return 0;
}

int file_import(const char* import_path) {

    FILE *fp;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;


    fp = fopen(import_path, "r");
    if (fp == NULL)
        return -EIO;

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu:\n", read);
        printf("%s\n", line);
    }

    fclose(fp);
    if (line)
        free(line);

    return 0;
}
