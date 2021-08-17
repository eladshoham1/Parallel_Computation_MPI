#ifndef __Gcd_Functions_H__
#define __Gcd_Functions_H__

#include <mpi.h>

typedef struct GcdNumbers
{
    int num1, num2, gcd;
} GcdNumbers;

GcdNumbers* readCouples(int *numOfCouples);
void gcd(GcdNumbers *gcdNumbers);
void calculateGcdArr(GcdNumbers *allGcdNumbers, int numOfCouples);
void printAllGcdNumbers(GcdNumbers *allGcdNumbers, int numOfCouples);
MPI_Datatype gcdNumbersMPIType();

#endif