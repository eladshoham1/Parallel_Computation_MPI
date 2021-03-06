#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gcdNumbers.h"

GcdNumbers* readCouples(int *numOfCouples)
{
    GcdNumbers *gcdNumbers;
    int i;
    char buffer[MAX_SIZE], ch;

    fgets(buffer, MAX_SIZE, stdin);
    *numOfCouples = atoi(buffer);
    gcdNumbers = (GcdNumbers*)doMalloc(*numOfCouples * sizeof(GcdNumbers));

    for (i = 0; i < *numOfCouples; i++)
    {
        fgets(buffer, MAX_SIZE, stdin);
        sscanf(buffer, "%d %d%c", &gcdNumbers[i].num1, &gcdNumbers[i].num2, &ch);
        
        if (gcdNumbers[i].num1 == 0 || gcdNumbers[i].num2 == 0 || ch != '\n')
        {
            printf("‫‪illegal‬‬ ‫‪input‬‬ ‫‪at‬‬ ‫‪line‬‬ %d\n", i + 2);
            free(gcdNumbers);
            return NULL;
        }
    }

    return gcdNumbers;
}

void gcd(GcdNumbers *gcdNumbers)
{
	int i;

	gcdNumbers->gcd = 1;
	for (i = 2; i < gcdNumbers->num1 || i < gcdNumbers->num2; i++)
	{
		if (gcdNumbers->num1 % i == 0 && gcdNumbers->num2 % i == 0)
            gcdNumbers->gcd = i;
	}
}

void calculateGcdArr(GcdNumbers *allGcdNumbers, int numOfCouples)
{
    int i;

    for (i = 0; i < numOfCouples; i++)
        gcd(allGcdNumbers + i);
}

void printAllGcdNumbers(GcdNumbers *allGcdNumbers, int numOfCouples)
{
    int i;

    for (i = 0; i < numOfCouples; i++)
        printf("%d %d\t gcd: %d\n", allGcdNumbers[i].num1, allGcdNumbers[i].num2, allGcdNumbers[i].gcd);
}

void createGcdNumbersType(MPI_Datatype* dataType)
{
    MPI_Type_contiguous(3, MPI_INT, dataType);
    MPI_Type_commit(dataType);
}

void* doMalloc(unsigned int nbytes) 
{
    void *p = malloc(nbytes);

    if (p == NULL) { 
        fprintf(stderr, "malloc failed\n"); 
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    return p;
}