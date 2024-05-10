#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include "numgen.c"
#include <math.h>
#define RANGESIZE 10
#define DATA 0
#define RESULT 1
#define FINISH 2
#define DEBUG

int ifPrime(unsigned long int number)
{
  unsigned long int sqrt_num = sqrt(number);
  for (unsigned long int i=2;i<=sqrt_num;i++)
  {
    if (number%i==0)
    {
      return 0;
    }
  }
  return 1;
}

int main(int argc,char **argv) {

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  //program input argument
  long inputArgument = ins__args.arg; 

  MPI_Status status;

  struct timeval ins__tstart, ins__tstop;
  int i;
  int myrank,nproc;
  unsigned long int *numbers;
  unsigned long int *resulttemp;
  MPI_Request *requests;
  int requestCount = 0;
  int requestCompleted;
  int isAnyrequestCompleted;

  MPI_Init(&argc,&argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  if(!myrank){
      gettimeofday(&ins__tstart, NULL);
	    numbers = (unsigned long int*)malloc(inputArgument * sizeof(unsigned long int));
  	  numgen(inputArgument, numbers);
  }

  // run your computations here (including MPI communication)
  int result=0;
  int range[2];
  unsigned long int *ranges;
  int sentcount = 0;
  int recvCount = 0;
  int a = 0, b = inputArgument;
  if (myrank==0) {

  requests = (MPI_Request *) malloc (3 * (nproc - 1) *
			sizeof (MPI_Request));
  if (!requests)
	{
            printf ("\nNot enough memory");
	    MPI_Finalize ();
	    return -1;
	}

  ranges = (unsigned long int *) malloc (2 * RANGESIZE * (nproc - 1) * sizeof (unsigned long int));
	if (!ranges)
	{
            printf ("\nNot enough memory");
	    MPI_Finalize ();
	    return -1;
	}

  resulttemp = (unsigned long int *) malloc ((nproc - 1) * sizeof (unsigned long int));
	if (!resulttemp)
	{
            printf ("\nNot enough memory");
	    MPI_Finalize ();
	    return -1;
	}

  range[0]=a;
  // first distribute some ranges to all slaves
  for(i=1;i<nproc;i++) {
    range[1]=range[0]+RANGESIZE;
    unsigned long int new_range[RANGESIZE];
    for (int j=0;j<RANGESIZE;j++)
    {
      if (range[0]+j >= b) new_range[j] = 4;
      else new_range[j] = numbers[range[0]+j];
    }
    #ifdef DEBUG
    printf("\nMaster sending range %d,%d to process %d",range[0],range[1],i);
    fflush(stdout);
    #endif
    // send it to process i
    MPI_Send(new_range,RANGESIZE,MPI_UNSIGNED_LONG,i,DATA,MPI_COMM_WORLD);
    sentcount++;
    range[0]=range[1];
  }

  // the first proccount requests will be for receiving, the latter ones for sending
	for (i = 0; i < 2 * (nproc - 1); i++)
  		requests[i] = MPI_REQUEST_NULL;	// none active at this point

    // start receiving for results from the slaves
	for (i = 1; i < nproc; i++)
            MPI_Irecv (&(resulttemp[i - 1]), 1, MPI_UNSIGNED_LONG, i, RESULT,
   			    MPI_COMM_WORLD, &(requests[i - 1]));


  // start sending new data parts to the slaves
	for (i = 1; i < nproc; i++)
	{
    range[1]=range[0]+RANGESIZE;
#ifdef DEBUG
	    printf ("\nMaster sending range %d,%d to process %d",
    			    range[0], range[1], i);
	    fflush (stdout);
#endif
    unsigned long int new_range[RANGESIZE];
    for (int j=0;j<RANGESIZE;j++)
    {
      if (range[0]+j >= b) ranges[RANGESIZE * (i-1) + j] = 4;
      else ranges[RANGESIZE * (i-1) + j] = numbers[range[0]+j];
    }
	  // send it to process i
	  MPI_Isend (&ranges[RANGESIZE * (i-1)], RANGESIZE, MPI_UNSIGNED_LONG, i, DATA,
 			    MPI_COMM_WORLD, &(requests[nproc - 2 + i]));
    sentcount++;
	  range[0] = range[1];
	}
  do {
    // distribute remaining subranges to the processes which have completed their parts
    MPI_Testany (2 * nproc - 2, requests, &requestCompleted, &isAnyrequestCompleted, MPI_STATUS_IGNORE);
    while(!isAnyrequestCompleted && range[0]< b)
    {
      result+=ifPrime(numbers[range[0]]);
      range[0]++;
      range[1]++;
      MPI_Testany (2 * nproc - 2, requests, &requestCompleted, &isAnyrequestCompleted,
			    MPI_STATUS_IGNORE);
    }
    if (!isAnyrequestCompleted) break;
    isAnyrequestCompleted = 0;
    //MPI_Waitany (2 * nproc - 2, requests, &requestCompleted, MPI_STATUS_IGNORE);
    if (requestCompleted < (nproc - 1))
    {
      result+=resulttemp[requestCompleted];
      recvCount++;
#ifdef DEBUG
		printf ("\nMaster received %d result %ld from process %d", recvCount,
				resulttemp[requestCompleted], requestCompleted + 1);
		fflush (stdout);
#endif
      // first check if the send has terminated
		  MPI_Wait (&(requests[nproc - 1 + requestCompleted]),
				MPI_STATUS_IGNORE);

    // now send some new data portion to this process

    range[1]=range[0]+RANGESIZE;
    
    unsigned long int new_range[RANGESIZE];
    
    for (int j=0;j<RANGESIZE;j++)
    {
      if (range[0]+j >= b) ranges[requestCompleted * RANGESIZE + j] = 4;
      else ranges[requestCompleted * RANGESIZE +j] = numbers[range[0]+j];
    }

      MPI_Isend (&ranges[RANGESIZE * requestCompleted], RANGESIZE, MPI_UNSIGNED_LONG,
				requestCompleted + 1, DATA, MPI_COMM_WORLD,
				&(requests[nproc - 1 + requestCompleted]));
      sentcount++;
      range[0] = range[1];
      MPI_Irecv (&(resulttemp[requestCompleted]), 1,
				MPI_UNSIGNED_LONG, requestCompleted + 1, RESULT,
				MPI_COMM_WORLD,
				&(requests[requestCompleted]));
    }
  } while (range[1]<b);

  range[0] = range[1];
  // now send the FINISHING ranges to the slaves
	// shut down the slaves
  for (i = 1; i < nproc; i++)
	{
#ifdef DEBUG
            printf("\nMaster sending FINISHING range %d,%d to process %d",
       			    range[0], range[1], i);
	    fflush (stdout);
#endif

	    unsigned long int new_range[RANGESIZE];
    for (int j=0;j<RANGESIZE;j++)
    {
      new_range[j]=0;
    }

      MPI_Isend (new_range, RANGESIZE, MPI_UNSIGNED_LONG,
				i, DATA, MPI_COMM_WORLD,
				&(requests[2 * nproc - 3 + i]));
	}

#ifdef DEBUG
	printf ("\nMaster before MPI_Waitall with total proccount=%d",
      			nproc);
	fflush (stdout);
#endif
	// now receive results from the processes - that is finalize the pending requests
        MPI_Waitall (3 * nproc - 3, requests, MPI_STATUSES_IGNORE);
#ifdef DEBUG
	printf ("\nMaster after MPI_Waitall with total proccount=%d",
      			nproc);
	fflush (stdout);
#endif
  // now simply add the results
	for (i = 0; i < (nproc - 1); i++)
	{
            result += resulttemp[i];
	}
  // now receive results for the initial sends
        for (i = 0; i < (nproc - 1); i++)
	{
#ifdef DEBUG
            printf ("\nMaster receiving result from process %d", i + 1);
	    fflush (stdout);
#endif
	    MPI_Recv (&(resulttemp[i]), 1, MPI_UNSIGNED_LONG, i + 1, RESULT,
  			    MPI_COMM_WORLD, &status);
	    result += resulttemp[i];
	    recvCount++;
#ifdef DEBUG
	    printf ("\nMaster received %d result %ld from process %d",
    			    recvCount, resulttemp[i], i + 1);
	    fflush (stdout);
#endif
	}
  // now display the result
  printf("\nHi, I am process 0, the result is %d\n",result);
}

else { //slave

  requests = (MPI_Request *) malloc (2 * sizeof (MPI_Request));

	if (!requests)
	{
            printf ("\nNot enough memory");
	    MPI_Finalize ();
	    return -1;
	}

	requests[0] = requests[1] = MPI_REQUEST_NULL;

  resulttemp = (unsigned long int *) malloc (2 * sizeof (unsigned long int));

	if (!resulttemp)
	{
            printf ("\nNot enough memory");
	    MPI_Finalize ();
	    return -1;
	}

  // first receive the initial data
  unsigned long int new_range[RANGESIZE];
  unsigned long int *second_range = (unsigned long int *) malloc (RANGESIZE * sizeof (unsigned long int));
  int finish = 0;
        MPI_Recv (new_range, RANGESIZE, MPI_UNSIGNED_LONG, 0, DATA, MPI_COMM_WORLD, &status);
#ifdef DEBUG
	printf ("\nSlave received numbers %ld,%ld, %ld", new_range[0], new_range[1], new_range[2]);
	fflush (stdout);
#endif
        while (!finish)
	{			
    if (new_range[0]==0 && new_range[1] == 0 && new_range[2] == 0)break;
    //finish = 1;
            // if there is some data to process
	    // before computing the next part start receiving a new data part
	    MPI_Irecv (second_range, RANGESIZE, MPI_UNSIGNED_LONG, 0, DATA, MPI_COMM_WORLD,
 			    &(requests[0]));

	    // compute my part
      resulttemp[1] = 0;
      for (int j=0;j<RANGESIZE;j++)
      {
        resulttemp[1] += ifPrime(new_range[j]);
        //if (new_range[j]!= 0 ) finish = 0;
      }
            // now finish receiving the new part
	    // and finish sending the previous results back to the master
            MPI_Waitall (2, requests, MPI_STATUSES_IGNORE);
	    resulttemp[0] = resulttemp[1];
      for (int j=0;j<RANGESIZE;j++)
      {
        new_range[j] = second_range[j];
      }
	    // and start sending the results back
            MPI_Isend (&resulttemp[0], 1, MPI_UNSIGNED_LONG, 0, RESULT,
 			    MPI_COMM_WORLD, &(requests[1]));
#ifdef DEBUG
	    printf("\nSlave just initiated send to master with result %ld",
       			    resulttemp[0]);
	    fflush (stdout);
#endif
	}
  #ifdef DEBUG
	    printf("Slave just ended work");
	    fflush (stdout);
#endif

	// now finish sending the last results to the master
	 MPI_Wait (&(requests[1]), MPI_STATUS_IGNORE);
   #ifdef DEBUG
	    printf("Slave just ended work forever");
	    fflush (stdout);
#endif
}


  if (!myrank) {
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }
  
  MPI_Finalize();


}
