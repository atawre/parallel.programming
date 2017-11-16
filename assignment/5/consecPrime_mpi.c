#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int isPrime(int n);

int main(int argc, char *argv[]){
    if(argc < 2) return 1;

    double start_time,end_time;
    int rank;
    int ntasks;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

    int size = strtol(argv[1], NULL, 10);

    if ((0!=ntasks%2) || (0!=size%ntasks)) {
        printf("An even number of tasks should be specified.\n");
        MPI_Finalize();
        exit(1);
    }

    int chunk = size/ntasks;
    int start = rank*chunk; 
    int end = start+chunk;
    int stride = ntasks*2;
    int prime = 0;
    int count = 0;
    int finalSum = 0;
    double *tarray;
    double wtime = 0;

    if(rank==0)
        start = 2;
    if(rank==(ntasks-1))
        end = size;

    start_time = MPI_Wtime();   /* Initialize start time */
    for(int i=start+1; i<end; i+=2){
        int new = isPrime(i);
        count += prime&new;
        prime = new;
    }

    MPI_Reduce(&count,&finalSum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    end_time=MPI_Wtime();
    wtime = end_time - start_time;

    if(rank == 0)
        tarray = (double *)malloc(ntasks*sizeof(double));

    MPI_Gather(&wtime, 1, MPI_DOUBLE, tarray, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(rank == 0){
        for(int i=0; i<ntasks; i++)
            printf("TaskID = %d, Wall Clock Time = %lf\n", i, tarray[i]);
	printf("---------------------------\n");
        printf("finalSum : %d\n", finalSum);
	printf("---------------------------\n");
    }

    MPI_Finalize();
    return 0;
}

int isPrime(int n){
	int notPrime = 0;
	if(n == 2) return 1;
	if(n%2 == 0) return 0;
	for(int i=3; i<=(int)sqrt(n); i+=2){
		notPrime = (n%i == 0);
		if(notPrime) return 0;
	}
	return !notPrime;	
}

