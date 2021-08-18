#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gcdNumbers.h"

enum tags { WORK, STOP };

void workerProcess(MPI_Datatype MPI_GCD_NUMBERS)
{
    GcdNumbers *gcdNumbers;
    int chunk, tag;
    MPI_Status status;
      
    do
    {
        MPI_Recv(&chunk, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gcdNumbers = (GcdNumbers*)doMalloc(chunk * sizeof(GcdNumbers));

        MPI_Recv(gcdNumbers, chunk, MPI_GCD_NUMBERS, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        tag = status.MPI_TAG;
        calculateGcdArr(gcdNumbers, chunk);
        MPI_Send(gcdNumbers, chunk, MPI_GCD_NUMBERS, ROOT, tag, MPI_COMM_WORLD);
        //printAllGcdNumbers(gcdNumbers, chunk);
        free(gcdNumbers);
    } while (tag != STOP);
}

void masterProcess(int numProcs, int chunk, MPI_Datatype MPI_GCD_NUMBERS)
{
    GcdNumbers *allGcdNumbers, *workGcdNumbers;
    int numOfCouples, workerId, jobsSent = 0, tag;
    MPI_Status status;

    allGcdNumbers = readCouples(&numOfCouples);
    if (!allGcdNumbers)
    {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    workGcdNumbers = (GcdNumbers*)doMalloc(chunk * sizeof(GcdNumbers));

    for (workerId = 1; workerId < numProcs; workerId++)
    {
        MPI_Send(&chunk, 1, MPI_INT, workerId, WORK, MPI_COMM_WORLD);
        MPI_Send(allGcdNumbers + jobsSent, chunk, MPI_GCD_NUMBERS, workerId, WORK, MPI_COMM_WORLD);
        jobsSent += chunk;
    }

    for (; jobsSent < numOfCouples; jobsSent += chunk)
    {
        MPI_Recv(workGcdNumbers, chunk, MPI_GCD_NUMBERS, MPI_ANY_SOURCE, WORK, MPI_COMM_WORLD, &status);
        printAllGcdNumbers(workGcdNumbers, chunk);

        if (numOfCouples - jobsSent <= (numProcs - 1))
            MPI_Send(allGcdNumbers + jobsSent, numOfCouples - jobsSent, MPI_GCD_NUMBERS, status.MPI_SOURCE, STOP, MPI_COMM_WORLD);
        else
            MPI_Send(allGcdNumbers + jobsSent, chunk, MPI_GCD_NUMBERS, status.MPI_SOURCE, WORK, MPI_COMM_WORLD);
    }

    for (workerId = 1; workerId < numProcs; workerId++)
    {
        MPI_Recv(workGcdNumbers, chunk, MPI_GCD_NUMBERS, MPI_ANY_SOURCE, STOP, MPI_COMM_WORLD, &status);
        printAllGcdNumbers(workGcdNumbers, chunk);
    }

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
        workerProcess(MPI_GCD_NUMBERS);
   
    MPI_Finalize();
    return 0;
}