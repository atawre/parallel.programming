#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define RANGE 100

void prefixSumCoarse(int *a, int *as, int *b, int *bc, int n, int id, int nt);
int prefixSum(int *a, int *b, int n, int id);


int main(int argc, char **argv){
	struct timespec tstart,tend; 
  float timer;

	if(argc < 3) return 1;
//generate test image (always starts with 0)
	int seed = strtol(argv[1], NULL, 10);
	int numRepeats = strtol(argv[2], NULL, 10);
	srand(seed);
	int count = 0;
	int image[numRepeats];
	int imagePsum[numRepeats];
	for(int i=0; i < numRepeats; i++){
		int size = (int)(rand()%RANGE)+1;
		count += size;
		image[i] = size;
		//printf("%d ", size);
	}
	//printf("\n");
	char *uncompressed = malloc(count*sizeof(char));
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	int nt = omp_get_max_threads();
	int *b = malloc(nt*sizeof(int));
	int *bc = malloc(nt*sizeof(int));
	
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		prefixSumCoarse(image, imagePsum, b, bc, numRepeats, id, nt);
		#pragma omp for
		for(int i=0; i<numRepeats; i++)
			for(int j=0; j<image[i]; j++)
				uncompressed[imagePsum[i]+ j] = i&1;
	}
	clock_gettime(CLOCK_MONOTONIC, &tend);
    timer = (tend.tv_sec-tstart.tv_sec) +
        (tend.tv_nsec-tstart.tv_nsec)*1.0e-9;
	printf("parallel runtime = %f\n", timer);

	int k = 0;
	int passed = 1;
	for(int i=0; i<numRepeats; i++){
		for(int j=0; j<image[i]; j++){
			int colour = i&1;
			if(colour != uncompressed[k]){ 
				printf("k: %d vs %d\n", k, colour, uncompressed[k]);
				passed = 0;
			}
			k++;
		}
	}
	if(passed)
		printf("result correct\n");
	return 0;
}

void prefixSumCoarse(int *a, int *as, int *b, int *bc, int n, int id, int nt){

}

int prefixSum(int *a, int *b, int n, int id){
	int *s;
	for(int j=1;j<n;j<<=1){
		if(id >= j) 
			b[id] = a[id-j]+a[id];
		else
			b[id] = a[id];
		s = a;
		a = b;
		b = s;
	#pragma omp barrier
	}
	if(id == 0)
		return 0;
	else
		return a[id-1];
}
