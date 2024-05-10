#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include "numgen.c"
#include <stdbool.h>

#define DATA 0
#define RESULT 1
#define DATA_SIZE 2

#define RANGE_SIZE 20

bool is_num_prime(unsigned long int number) {
    if (number < 2) {
        return false;
    }
    for (int i = 2; i <= number / 2; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

void copy_array(unsigned long int *input_array, unsigned long int *output_array, int start_indx, int end_indx) {
    for (int i = 0; i < RANGE_SIZE; i++) {
        int index = start_indx + i;
        if (index <= end_indx) {
            output_array[i] = input_array[index];
        } else {
            output_array[i] = 0;
        }
    }
}

bool is_array_correct(unsigned long int *array) {
    for (int i = 0; i < RANGE_SIZE; i++) {
        if (array[i] != 0) {
            return true;
        }
    }
    return false;
}


int main(int argc, char **argv) {

    Args ins__args;
    parseArgs(&ins__args, &argc, argv);

    //program input argument
    long inputArgument = ins__args.arg;

    struct timeval ins__tstart, ins__tstop;

    int myrank, nproc;
    unsigned long int *numbers, result = 0;

    MPI_Status status;


    MPI_Init(&argc, &argv);

    // obtain my rank
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // and the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    if (!myrank) {
        // Master
        // printf("I'm Master\n");
        numbers = (unsigned long int *) malloc(inputArgument * sizeof(unsigned long int));
        numgen(inputArgument, numbers);
//        for (int i = 0; i < inputArgument; i++) {
//            printf("number %d: %lu\n", i, numbers[i]);
//        }
        gettimeofday(&ins__tstart, NULL);

        // initialization of master
        unsigned long int range[2];
        MPI_Request *requests = (MPI_Request *) malloc(3 * (nproc - 1) * sizeof(MPI_Request));
        unsigned long int *result_temp = malloc((nproc - 1) * sizeof(unsigned long int));
        unsigned long int **data = malloc((nproc - 1) * sizeof(void *));
        for (int i = 0; i < nproc - 1; i++) {
            data[i] = malloc(RANGE_SIZE * sizeof(unsigned long int *));
        }


        range[0] = 0;
        for (int i = 1; i < nproc; i++) {
            range[1] = range[0] + RANGE_SIZE;
            if (range[1] > inputArgument) {
                range[1] = inputArgument;
            }

            copy_array(numbers, data[i - 1], range[0], range[1]);
           printf("Master: Sending to slave nr.%d: ", i);
           for (int j = 0; j < RANGE_SIZE; j++) {
               printf("%lu, ", data[i - 1][j]);
           }
           printf("\n");
            MPI_Send(data[i - 1], RANGE_SIZE, MPI_UNSIGNED_LONG, i, DATA, MPI_COMM_WORLD);
            range[0] = range[1];
        }
        for (int i = 0; i < 2 * (nproc - 1); i++) {
            requests[i] = MPI_REQUEST_NULL;
        }
        // first i to receive
        for (int i = 0; i < nproc - 1; i++) {
            MPI_Irecv(result_temp + i, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, RESULT,
                      MPI_COMM_WORLD, requests + i);
        }
        for (int i = 1; i < nproc; i++) {
            range[1] = range[0] + RANGE_SIZE;
            if (range[1] > inputArgument) {
                range[1] = inputArgument;
            }

            copy_array(numbers, data[i - 1], range[0], range[1]);
           printf("Master: ISending to slave nr.%d: ", i);
           for (int j = 0; j < RANGE_SIZE; j++) {
               printf("%lu, ", data[i - 1][j]);
           }
           printf("\n");
            MPI_Isend(data, RANGE_SIZE, MPI_UNSIGNED_LONG, i, DATA, MPI_COMM_WORLD, requests + nproc - 2 + i);
            range[0] = range[1];
        }

        int completed_request_index;
        while (range[1] < inputArgument) {
            MPI_Waitany(2 * nproc - 2, requests, &completed_request_index, MPI_STATUS_IGNORE);
            if (completed_request_index < nproc - 1) {
                result += result_temp[completed_request_index];

                MPI_Wait(requests + nproc - 1 + completed_request_index,
                         MPI_STATUS_IGNORE);
                range[1] = range[0] + RANGE_SIZE;
                if (range[1] > inputArgument) {
                    range[1] = inputArgument;
                }
                copy_array(numbers, data[completed_request_index], range[0], range[1]);
               printf("Master: ISending to slave nr.%d: ", completed_request_index + 1);
               for (int j = 0; j < RANGE_SIZE; j++) {
                   printf("%lu, ", data[completed_request_index][j]);
               }
                MPI_Isend(data[completed_request_index], RANGE_SIZE, MPI_UNSIGNED_LONG, completed_request_index + 1,
                          DATA, MPI_COMM_WORLD,
                          requests + nproc - 1 + completed_request_index);

                range[0] = range[1];
                MPI_Irecv(result_temp + completed_request_index, 1, MPI_UNSIGNED_LONG, completed_request_index +
                                                                                       1, RESULT, MPI_COMM_WORLD,
                          requests + completed_request_index);


            }
        }
        unsigned long int *empty_array = malloc(RANGE_SIZE * sizeof(unsigned long int));
        for (int i = 0; i < RANGE_SIZE; i++) {
            empty_array[i] = 0;
        }
        for (int i = 0; i < nproc - 1; i++) {
            MPI_Isend(empty_array, RANGE_SIZE, MPI_UNSIGNED_LONG, i + 1, DATA, MPI_COMM_WORLD,
                      requests + 2 * (nproc - 1) + i);
        }
        MPI_Waitall(3 * (nproc - 1), requests, MPI_STATUSES_IGNORE);
        for (int i = 0; i < (nproc - 1); i++) {
            result += result_temp[i];
        }
        for (int i = 1; i < nproc; i++) {
            MPI_Recv(result_temp + i - 1, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status);
            result += result_temp[i - 1];
//            printf("Master: slave nr.%d sent result: %lu\n", iresult_temp[i - 1]);
        }

        printf("\nMaster: the result is %lu\n", result);
        free(empty_array);

    } else {
        //Slaves
       printf("I'm slave nr.%d\n", myrank);

        MPI_Request *requests = (MPI_Request *) malloc(2 * sizeof(MPI_Request));
        requests[0] = requests[1] = MPI_REQUEST_NULL;
        unsigned long int *data = malloc(RANGE_SIZE * sizeof(unsigned long int));
        unsigned long int *idata = malloc(RANGE_SIZE * sizeof(unsigned long int));
        unsigned long int *result = malloc(2 * sizeof(unsigned long int));
        result[0] = result[1] = 0;

        MPI_Recv(data, RANGE_SIZE, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, DATA,
                 MPI_COMM_WORLD, &status);

        while (is_array_correct(data)) {
            int result = 0;
            MPI_Irecv(data, RANGE_SIZE, MPI_UNSIGNED_LONG, 0, DATA, MPI_COMM_WORLD, requests);
            for (int i = 0; i < RANGE_SIZE; i++) {
               printf("Slave nr.%d checks number: %lu\n", myrank, data[i]);
                if (is_num_prime(data[i])) {
//                    printf("It's prime\n");
                    result++;
                }
            }
            MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
            // data = idata;
            MPI_Isend(&result, 1, MPI_UNSIGNED_LONG, 0, RESULT, MPI_COMM_WORLD,
                      requests + 1);
        }
        MPI_Wait(requests + 1, MPI_STATUS_IGNORE);
        free(data);
    }


    if (!myrank) {
        gettimeofday(&ins__tstop, NULL);
        ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
    }

    MPI_Finalize();

}
