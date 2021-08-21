#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gcdNumbers.h"

enum tags { WORK, STOP };

void workerProcess(int myRank, MPI_Datatype MPI_GCD_NUMBERS)
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
        //printf("\n procs num %d and tag is %d\n", myRank, tag);
        //printAllGcdNumbers(gcdNumbers, chunk);
        MPI_Send(gcdNumbers, chunk, MPI_GCD_NUMBERS, ROOT, tag, MPI_COMM_WORLD);
        free(gcdNumbers);
    } while (tag != STOP);
    
}

void masterProcess(int numProcs, int chunk, MPI_Datatype MPI_GCD_NUMBERS)
{
    GcdNumbers *allGcdNumbers;
    int numOfCouples, workerId, jobSent = 0, jobDone = 0, tag;
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
        MPI_Send(&chunk, 1, MPI_INT, workerId, WORK, MPI_COMM_WORLD);
        MPI_Send(allGcdNumbers + jobDone, chunk, MPI_GCD_NUMBERS, workerId, WORK, MPI_COMM_WORLD);
        jobDone += chunk;
    }
    int rm = 0, temp = 0, temp2 = 0;
    for (workerId = 1, jobSent = 0; jobSent < numOfCouples; workerId++, jobSent += chunk)
    {
        workerId = (workerId % numProcs) != 0 ? workerId % numProcs : 1;
        MPI_Recv(allGcdNumbers + jobSent, chunk, MPI_GCD_NUMBERS, workerId, WORK, MPI_COMM_WORLD, &status);

        if (jobDone + jobSent + (numProcs - 1) * chunk >= numOfCouples)
        {
            if (tag != STOP)
            {
                tag = STOP;
                chunk = (numOfCouples - (jobDone + jobSent)) / (numProcs - 1);
                rm = (numOfCouples - (jobDone + jobSent)) % (numProcs - 1);
                temp = jobDone + jobSent;
            }
        } 
        else
            tag = WORK;

        if (workerId == numProcs - 1 && tag == STOP)
        {
            temp2 = chunk + rm;
            MPI_Send(&temp2, 1, MPI_INT, workerId, tag, MPI_COMM_WORLD);
            MPI_Send(allGcdNumbers + jobDone + jobSent, temp2, MPI_GCD_NUMBERS, workerId, tag, MPI_COMM_WORLD);
            jobDone += temp2;
        }
        else
        {
            MPI_Send(&chunk, 1, MPI_INT, workerId, tag, MPI_COMM_WORLD);
            MPI_Send(allGcdNumbers + jobDone + jobSent, chunk, MPI_GCD_NUMBERS, workerId, tag, MPI_COMM_WORLD);
            jobDone += chunk;
        }
    }

    for (workerId = 1; workerId < numProcs; workerId++)
    {
        if (workerId != numProcs - 1)
        {
            MPI_Recv(allGcdNumbers + temp, chunk, MPI_GCD_NUMBERS, workerId, STOP, MPI_COMM_WORLD, &status);
            temp += chunk;
        }
        else
        {
            MPI_Recv(allGcdNumbers + temp, temp2, MPI_GCD_NUMBERS, workerId, STOP, MPI_COMM_WORLD, &status);
            temp += temp2;
        }
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
        workerProcess(myRank, MPI_GCD_NUMBERS);
   
    MPI_Finalize();
    return 0;
}