#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "gcdNumbers.h"

enum chunk { N = 1 };

int main(int argc, char *argv[])
{
    GcdNumbers *allGcdNumbers, workGcdNumbers[N];
    int myRank, numProcs, numOfCouples, jobsDone = 0;
    double time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Datatype MPI_GCD_NUMBERS = gcdNumbersMPIType();

    if (myRank == ROOT)
    {
        allGcdNumbers = readCouples(&numOfCouples);
        if (!allGcdNumbers)
        {
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            exit(EXIT_FAILURE);
        }

        time = MPI_Wtime();
    }

    MPI_Bcast(&numOfCouples, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    while (jobsDone < numOfCouples)
    {
        MPI_Scatter(allGcdNumbers + jobsDone, N, MPI_GCD_NUMBERS, workGcdNumbers, N, MPI_GCD_NUMBERS, ROOT, MPI_COMM_WORLD);
        jobsDone += N * numProcs;
        calculateGcdArr(workGcdNumbers, N);
        MPI_Gather(workGcdNumbers, N, MPI_GCD_NUMBERS, allGcdNumbers, N, MPI_GCD_NUMBERS, ROOT, MPI_COMM_WORLD);
    }

    if (myRank == ROOT)
    {
        printf("Run time: %lf\n", MPI_Wtime() - time);
        printAllGcdNumbers(allGcdNumbers, numOfCouples);
        free(allGcdNumbers);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}