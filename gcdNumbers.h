#ifndef __Gcd_Functions_H__
#define __Gcd_Functions_H__

#include <mpi.h>

enum str { MAX_SIZE = 100 };
enum ranks { ROOT };

typedef struct GcdNumbers
{
    int num1, num2, gcd;
} GcdNumbers;

GcdNumbers* readCouples(int *numOfCouples);
void gcd(GcdNumbers *gcdNumbers);
void calculateGcdArr(GcdNumbers *allGcdNumbers, int numOfCouples);
void printAllGcdNumbers(GcdNumbers *allGcdNumbers, int numOfCouples);
MPI_Datatype gcdNumbersMPIType();
void* doMalloc(unsigned int nbytes);

#endif