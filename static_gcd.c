#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gcdFunctions.h"

enum ranks { ROOT, N = 5 };

int main(int argc, char *argv[])
{
    int myRank, numProcs, numOfCouples, jobsDone = 0;
    GcdNumbers *allGcdNumbers, workGcdNumbers[N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Datatype gcdNumbersType = gcdNumbersMPIType();

    if (myRank == ROOT)
    {
        scanf("%d", &numOfCouples);
        allGcdNumbers = (GcdNumbers*)malloc(numOfCouples * sizeof(GcdNumbers));
        if (!allGcdNumbers)
            exit(EXIT_FAILURE);

        readCouples(allGcdNumbers, numOfCouples);
    }

    MPI_Bcast(&numOfCouples,1,MPI_INT,ROOT,MPI_COMM_WORLD);

    while (jobsDone < numOfCouples)
    {
        MPI_Scatter(allGcdNumbers + jobsDone, N, gcdNumbersType, workGcdNumbers, N, gcdNumbersType, ROOT, MPI_COMM_WORLD);
        jobsDone += N * numProcs;
        calculateGcdArr(workGcdNumbers, N);
        MPI_Gather(workGcdNumbers, N, gcdNumbersType, allGcdNumbers, N, gcdNumbersType, ROOT, MPI_COMM_WORLD);
    }

    if (myRank == ROOT)
    {
        printAllGcdNumbers(allGcdNumbers, numOfCouples);
        free(allGcdNumbers);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}