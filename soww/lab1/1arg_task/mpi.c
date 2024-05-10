#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

int main(int argc,char **argv) {

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  //program input argument
  long inputArgument = ins__args.arg; 

  struct timeval ins__tstart, ins__tstop;

  int myrank,nproc;

  double precision = 0.000001;
  double begin = 0.0;
  double end = 3.14159;

  double myBegin, myEnd;

  double result = 0.0;
  double integral;
  
  MPI_Init(&argc,&argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  if(!myrank)
      gettimeofday(&ins__tstart, NULL);


  // run your computations here (including MPI communication)
    integral = 0.0;
    myBegin = begin + (double) myrank * (end - begin) / nproc;
    myEnd = begin + (double) (myrank + 1) * (end - begin) / nproc;

    printf("rank %d myBegin: %.3f myEnd: %.3f\n", myrank, myBegin, myEnd);

    for (double d = myBegin; d < myEnd; d += precision) {
      integral += cos(d) * precision;
    }

    // merge the results of each process
    MPI_Reduce(&integral, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myrank == 0) {
        printf("\nresult = %.4f\n", result);
    }

  // synchronize/finalize your computations

  if (!myrank) {
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }
  
  MPI_Finalize();

}
