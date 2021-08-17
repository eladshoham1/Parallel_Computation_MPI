#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gcdNumbers.h"

enum ranks { ROOT };

int main(int argc, char *argv[])
{
    int myRank, numProcs, numOfCouples, workNumOfCouples;
    GcdNumbers *allGcdNumbers, *workGcdNumbers;
    
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

        workNumOfCouples = numOfCouples / numProcs;
    }

    MPI_Bcast(&workNumOfCouples, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    workGcdNumbers = (GcdNumbers*)malloc(workNumOfCouples * sizeof(GcdNumbers));
    if (!workGcdNumbers)
    {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    MPI_Scatter(allGcdNumbers, workNumOfCouples, MPI_GCD_NUMBERS, workGcdNumbers, workNumOfCouples, MPI_GCD_NUMBERS, ROOT, MPI_COMM_WORLD);
    calculateGcdArr(workGcdNumbers, workNumOfCouples);
    MPI_Gather(workGcdNumbers, workNumOfCouples, MPI_GCD_NUMBERS, allGcdNumbers, workNumOfCouples, MPI_GCD_NUMBERS, ROOT, MPI_COMM_WORLD);

    if (myRank == ROOT)
    {
        if (numOfCouples % numProcs > 0)
            calculateGcdArr(allGcdNumbers + workNumOfCouples * numProcs, numOfCouples % numProcs);

        printAllGcdNumbers(allGcdNumbers, numOfCouples);
        free(allGcdNumbers);
    }
    free(workGcdNumbers);

    MPI_Finalize();
    return EXIT_SUCCESS;
}