#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include "numgen.c"
#include "math.h"

int isPrime(long num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1;
}

#define RANGE_SIZE 10

int main(int argc,char **argv) {

    Args ins__args;
    parseArgs(&ins__args, &argc, argv);

    //set number of threads
    omp_set_num_threads(ins__args.n_thr);

    //program input argument
    long inputArgument = ins__args.arg; 
    unsigned long int *numbers = (unsigned long int*)malloc(inputArgument * sizeof(unsigned long int));
    numgen(inputArgument, numbers);

    struct timeval ins__tstart, ins__tstop;
    gettimeofday(&ins__tstart, NULL);

    // run your computations here (including OpenMP stuff)

    //lock object - usable for synchronization
    omp_lock_t writelock;
    omp_init_lock(&writelock);

    int result = 0;

    int processResult = 0;

    int rangeSize = inputArgument / ins__args.n_thr;
    printf("range size %d\n\n", rangeSize);

    #pragma omp parallel shared(result)
    {
        int threadId = omp_get_thread_num();
        
        int start = threadId * rangeSize;
        int end = (threadId + 1) * rangeSize;
        if (threadId == ins__args.n_thr - 1) end = inputArgument;
        printf("thread %d start %d end %d\n", threadId, start, end);

        int localResult = 0;
        for (int i = start; i < end; i++) {
            if (isPrime(numbers[i])) localResult += 1;
        }
        
        #pragma omp atomic update
        result += localResult;
    }

    printf("\nresult = %d\n", result);


    // synchronize/finalize your computations
    free(numbers);
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
}
