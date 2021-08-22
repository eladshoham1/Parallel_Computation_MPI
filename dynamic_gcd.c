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

        free(gcdNumbers);
    } while (tag != STOP);
}

void masterProcess(int numProcs, int chunk, MPI_Datatype MPI_GCD_NUMBERS)
{
    GcdNumbers *allGcdNumbers;
    int numOfCouples, workerId, tag, remainder = 0;
    double time;
    MPI_Status status;

    allGcdNumbers = readCouples(&numOfCouples);
    if (!allGcdNumbers)
    {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    time = MPI_Wtime();
    
    for (int jobSent = 0; tag != STOP && jobSent < numOfCouples; jobSent += (numProcs - 1) * chunk)
    {
        if (numOfCouples <= jobSent + (numProcs - 1) * chunk)
        {
            tag = STOP;
            chunk = (numOfCouples - jobSent) / (numProcs - 1);
            remainder = (numOfCouples - jobSent) % (numProcs - 1);
        }
        else
            tag = WORK;

        for (workerId = 1; workerId < numProcs; workerId++)
        {
            MPI_Send(&chunk, 1, MPI_INT, workerId, tag, MPI_COMM_WORLD);
            MPI_Send(allGcdNumbers + jobSent + (workerId - 1) * chunk, chunk, MPI_GCD_NUMBERS, workerId, tag, MPI_COMM_WORLD);
        }
        
        for (workerId = 1; workerId < numProcs; workerId++)
            MPI_Recv(allGcdNumbers + jobSent + (workerId - 1) * chunk, chunk, MPI_GCD_NUMBERS, workerId, tag, MPI_COMM_WORLD, &status);
    }

    if (remainder != 0)
        calculateGcdArr(allGcdNumbers + (numOfCouples - remainder), remainder);

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
        workerProcess(MPI_GCD_NUMBERS);
   
    MPI_Finalize();
    return 0;
}