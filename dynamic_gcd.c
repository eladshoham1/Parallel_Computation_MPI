#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*enum ranks{ROOT};
enum tags{WORK, STOP};

int* readNumbers(int size)
{
    int *arr, i;

    arr = (int*)malloc(size * sizeof(int));
    if (!arr)
        return NULL;

    printf("Enter %d numbers: ", size);
    for (i = 0; i < size; i++)
        scanf("%d", &arr[i]);

    return arr;
}

int findPrime(int *arr, int size)
{
    int i, j, p, count = 0;

    for (i = 0; i < size; i++)
    {
        p = 1;
        for (j = 2; p != 0 && j < arr[i]; j++)
        {
            if (arr[i] % j == 0)
                p = 0;
        }
        if (p == 1)
            count++;
    }

    return count;
}

void workerProcess()
{
    int arr[N], countPrime,tag;
    MPI_Status status;
    do
    {
        MPI_Recv(arr,N,MPI_INT,ROOT,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        tag = status.MPI_TAG;
        countPrime = findPrime(arr,N);
        MPI_Send(&countPrime,1,MPI_INT,ROOT,tag,MPI_COMM_WORLD);

    } while (tag != STOP);
}

void masterProcess(int num_procs)
{
    MPI_Status status;
    int *arr , arrSize, worker_id, jobs_sent = 0, tag;
    int localCount = 0, globalCount = 0;

    printf("Enter number of numbers: ");
    scanf("%d", &arrSize);
    arr = readNumbers(arrSize);
    if (!arr)
        exit(1);
    double t = MPI_Wtime();
    //start workers
    for(worker_id=1;worker_id < num_procs;worker_id++)
    {
        MPI_Send(arr+jobs_sent,N,MPI_INT,worker_id,WORK,MPI_COMM_WORLD);
        jobs_sent += N;
    }

    //recv and send more work
    printf("arrsize = %d\n",arrsize);
    for(;jobs_sent < arrsize;jobs_sent+=N)
    {
        if(arrsize - jobs_sent <= (num_procs-1)*N)
            tag = STOP;
        else
            tag = WORK;
        MPI_Recv(&localCount,1,MPI_INT,MPI_ANY_SOURCE,WORK,MPI_COMM_WORLD,&status);
        MPI_Send(arr+jobs_sent,N,MPI_INT,status.MPI_SOURCE,tag,MPI_COMM_WORLD);
        globalCount += localCount;
    }

    //terminate
    for(worker_id=1;worker_id < num_procs;worker_id++)
    {
        MPI_Recv(&localCount,1,MPI_INT,MPI_ANY_SOURCE,STOP,MPI_COMM_WORLD,&status);
        globalCount += localCount;
    }
    printf("Parallel prime count is %d, time %lf\n",globalCount,MPI_Wtime()-t);
    t = MPI_Wtime();
    globalCount = findPrime(arr,arrsize);
    printf("Sequential prime count is %d, time %lf\n",globalCount,MPI_Wtime()-t);
    free(arr);
}*/

int main(int argc, char *argv[])
{
    /*int myRank, numProcs, chunk;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs); 

    chunk = argc > 1 ? atoi(argv[1]) : 1;
    if (myRank == ROOT)
        masterProcess(numProcs, chunk);
    else
        workerProcess();

    MPI_Finalize();*/
    return 0;
}