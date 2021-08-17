#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gcdNumbers.h"

GcdNumbers* readCouples(int *numOfCouples)
{
    GcdNumbers *gcdNumbers;
    int i;
    char buffer[100], ch;

    fgets(buffer, 100, stdin);
    *numOfCouples = atoi(buffer);
    gcdNumbers = (GcdNumbers*)malloc(*numOfCouples * sizeof(GcdNumbers));
    if (!gcdNumbers)
        return NULL;

    for (i = 0; i < *numOfCouples; i++)
    {
        fgets(buffer, 100, stdin);
        sscanf(buffer, "%d %d%c", &gcdNumbers[i].num1, &gcdNumbers[i].num2, &ch);
        
        if (gcdNumbers[i].num1 == 0 || gcdNumbers[i].num2 == 0 || ch != '\n')
        {
            printf("‫‪illegal‬‬ ‫‪input‬‬ ‫‪at‬‬ ‫‪line‬‬ %d\n", i + 2);
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

MPI_Datatype gcdNumbersMPIType()
{
    GcdNumbers gcdNumbers;
    int blocklen[3] = { 1, 1, 1 };
    MPI_Aint disp[3];
    MPI_Datatype oldTypes[3] = { MPI_INT, MPI_INT, MPI_INT };
    MPI_Datatype gcdNumbersType;

    disp[0] = (char*)&gcdNumbers.num1 - (char*)&gcdNumbers;
    disp[1] = (char*)&gcdNumbers.num2 - (char*)&gcdNumbers;
    disp[2] = (char*)&gcdNumbers.gcd - (char*)&gcdNumbers;
    MPI_Type_create_struct(3, blocklen, disp, oldTypes, &gcdNumbersType);
    MPI_Type_commit(&gcdNumbersType);

    return gcdNumbersType;
}