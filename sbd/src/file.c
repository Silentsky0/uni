#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "common/status.h"

int str_to_record(struct record *rec, char* str);

void file_init(struct file* file) {
    file->num_records = 0;
    file->num_overflow_records = 0;
    file->records = NULL;
    file->overflow = malloc(100 * sizeof(struct record)); // TODO fix simplified version
    file->ov_start = file->overflow;
}

int file_append_record(struct file *file, struct record *record) {

    memcpy((void *)file->ov_start, (void *)record, sizeof(struct record));

    printf("%p %p\n", file->ov_start, record);

    char *byte = (char *)record;
    for (int i = 0; i < 32; i++) {
        printf("%d\n", *byte);
        byte++;
    }

    //memcpy(file->ov_start, (void *)&record, sizeof(struct record));
    file->ov_start += sizeof(struct record);
    file->records += 1;

    record_print(*record);

    free(record);

    return 0;
}

int file_import( struct file *file, const char* import_path) {

    FILE *fp;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;


    fp = fopen(import_path, "r");
    if (fp == NULL)
        return -EIO;

    while ((read = getline(&line, &len, fp)) != -1) {
        struct record record_to_add;
        str_to_record(&record_to_add, line);
        record_print(record_to_add);
        file_append_record(file, &record_to_add);
    }

    fclose(fp);
    if (line)
        free(line);

    return 0;
}

// TODO may be in utils
struct record str_to_record(char* str){

    struct record ret;

    char * token = strtok(str, " ");
    ret.data.name = token;

    token = strtok(NULL, " ");
    ret.data.surname = token;

    token = strtok(NULL, " ");
    char series[4];
    char number[6];
    strncpy(series, token, 3);
    strncpy(number, &token[3], 5);
    series[3] = '\0';
    number[5] = '\0';
    ret.id.identity_number = atoi(number);
    ret.id.identity_series[0] = 'a';
    ret.id.identity_series[1] = 'b';
    ret.id.identity_series[2] = 'c';
    ret.id.identity_series[3] = '\0';
    //record_print(&ret);

    return ret;
}

int file_print(struct file *file) {
    printf("--- main file section ---\n");

    for(int i = 0; i < file->num_overflow_records; i++) {
        record_print(file->records[i]);
    }

    printf("--- overflow file section ---\n");
    
    for(int i = 0; i < file->num_overflow_records; i++) {
        record_print(file->overflow[i]);
    }

    return 0;
}
