#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "gcdNumbers.h"

enum size { N = 16 };

int main(int argc, char *argv[])
{
    GcdNumbers *allGcdNumbers, workGcdNumbers[N];
    int myRank, numProcs, numOfCouples, jobsDone = 0;
    double time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Datatype gcdNumbersType;
    createGcdNumbersType(&gcdNumbersType);

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

    while (jobsDone + N * numProcs <= numOfCouples)
    {
        MPI_Scatter(allGcdNumbers + jobsDone, N, gcdNumbersType, workGcdNumbers, N, gcdNumbersType, ROOT, MPI_COMM_WORLD);
        calculateGcdArr(workGcdNumbers, N);
        MPI_Gather(workGcdNumbers, N, gcdNumbersType, allGcdNumbers + jobsDone, N, gcdNumbersType, ROOT, MPI_COMM_WORLD);
        jobsDone += N * numProcs;
    }

    if (myRank == ROOT)
    {
        if (numOfCouples - jobsDone > 0)
            calculateGcdArr(allGcdNumbers + jobsDone, numOfCouples - jobsDone);

        printf("Run time: %lf\n", MPI_Wtime() - time);
        printAllGcdNumbers(allGcdNumbers, numOfCouples);
        free(allGcdNumbers);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}