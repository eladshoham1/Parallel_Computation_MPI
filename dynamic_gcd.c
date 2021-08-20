#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gcdNumbers.h"

enum tags { WORK, STOP };

void workerProcess(int chunk, MPI_Datatype MPI_GCD_NUMBERS)
{
    GcdNumbers *gcdNumbers = (GcdNumbers*)doMalloc(chunk * sizeof(GcdNumbers));
    int tag;
    MPI_Status status;

    do
    {
        MPI_Recv(gcdNumbers, chunk, MPI_GCD_NUMBERS, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        tag = status.MPI_TAG;
        calculateGcdArr(gcdNumbers, chunk);
        printAllGcdNumbers(gcdNumbers, chunk);
        MPI_Send(gcdNumbers, chunk, MPI_GCD_NUMBERS, ROOT, tag, MPI_COMM_WORLD);
    } while (tag != STOP);
    
    free(gcdNumbers);
}

void masterProcess(int numProcs, int chunk, MPI_Datatype MPI_GCD_NUMBERS)
{
    GcdNumbers *allGcdNumbers;
    int numOfCouples, workerId, jobSent, jobDone = 0, totalJob = 0, tag;
    double time;
    MPI_Status status;

    allGcdNumbers = readCouples(&numOfCouples);
    if (!allGcdNumbers)
    {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    time = MPI_Wtime();
    for (workerId = 1; workerId < numProcs; workerId++)
    {
        MPI_Send(allGcdNumbers + jobDone, chunk, MPI_GCD_NUMBERS, workerId, WORK, MPI_COMM_WORLD);
        jobDone += chunk;
    }

    for (jobSent = 0; jobSent < numOfCouples; jobSent += chunk)
    {
        MPI_Recv(allGcdNumbers + jobSent, chunk, MPI_GCD_NUMBERS, MPI_ANY_SOURCE, WORK, MPI_COMM_WORLD, &status);

        if (jobDone + jobSent <= numOfCouples)
        {
            tag = STOP;
            chunk = numOfCouples - (jobDone + jobSent);
        }
        else
            tag = WORK;

        MPI_Send(allGcdNumbers + jobDone + jobSent, chunk, MPI_GCD_NUMBERS, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
        if (tag != STOP)
            totalJob = jobDone + jobSent;
    }

    for (workerId = 1; workerId < numProcs; workerId++)
    {
        MPI_Recv(allGcdNumbers + totalJob, chunk, MPI_GCD_NUMBERS, workerId, STOP, MPI_COMM_WORLD, &status);
        totalJob += chunk;
    }

    printf("Run time: %lf\n", MPI_Wtime() - time);
    printAllGcdNumbers(allGcdNumbers, numOfCouples);
    free(allGcdNumbers);
}

int main(int argc, char *argv[])
{
    int myRank, numProcs, chunk = argc >= 2 ? atoi(argv[1]) : 2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs); 
    MPI_Datatype MPI_GCD_NUMBERS = gcdNumbersMPIType();

    if (myRank == ROOT)
        masterProcess(numProcs, chunk, MPI_GCD_NUMBERS);
    else
        workerProcess(chunk, MPI_GCD_NUMBERS);
   
    MPI_Finalize();
    return 0;
}