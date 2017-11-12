#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <time.h>
#include <string.h>

void up(int a[], int n){
	int upper_bound = log2(n);
	for (int k = 0; k < upper_bound; k++){
		int j = exp2(k);
		int stride = 2*j;
		int m = n/stride;
		a[2*j-1:m:stride] += a[j-1:m:stride];
	}
}

void down(int a[], int n, int* tmp){
	for (int k = log2(n)-1; k >= 0; k--){
		int x = exp2(k);
		int stride = exp2(k+1);
		int m = n/stride;
		tmp[0:m:stride] = a[x-1:m:stride];
		a[x-1:m:stride] = a[stride-1:m:stride];
		a[stride-1:m:stride] += tmp[0:m:stride];
	}
}

//Blelloch Prefix Sum
void ex_pref_sum(int a[], int n, int* tmp){
	up(a,n);
	a[n-1]=0;
	down(a,n,tmp);
}


//Sequencial Prefix Sum
void prefixOnHost(int a[], int n){
	int* b = (int*)malloc(sizeof(int)*n);
	memcpy(b,a,sizeof(int)*n);
	int sum = 0;
	for(int i=0; i<n; i++){
		sum += b[i];
		b[i] = sum;
		a[i] = b[i] - a[i];
	}
	free(b);
}


//gcc -fcilkplus Blelloch_PrefixSum.c -O2 -o Blelloch_PrefixSum -ftree-vectorize -fopt-info-vec-optimized -lrt
int main(int argc, char const *argv[])
{
	struct timespec tstart, tend;
	float time;
	if(2!=argc){
		printf("Usage : ./r_prefix <arraySize> \n");
		exit(1);
	}
	int n = atoi(argv[1]);
	srand((unsigned)0);
	int* a_s = (int*)malloc(sizeof(int)*n);
	int* a_p = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i < n; ++i)
	{
		a_s[i] = rand() % n;
		a_p[i] = a_s[i];
	}

	//sequential prefix sum for verification and timing
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	prefixOnHost(a_s, n);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	time = (tend.tv_sec-tstart.tv_sec) + (tend.tv_nsec-tstart.tv_nsec)*1.0e-9;
	printf("sequential prefix sum time in seconds: %f\n", time);

	int k = log2(n);
	if(exp2(k) < n){
		int m = exp2(k+1);
		int* b = (int*)malloc(sizeof(int)*m);
		int* tmp = (int*)malloc(sizeof(int)*m);
		memcpy(b,a_p,sizeof(int)*n);
		memset(b+n,0,m-n);
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		ex_pref_sum(b,m,tmp);
		clock_gettime(CLOCK_MONOTONIC, &tend);
		memcpy(a_p,b,sizeof(int)*n);
		free(b);
		free(tmp);
	}
	else{
		int* tmp = (int*)malloc(sizeof(int)*n);
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		ex_pref_sum(a_p,n,tmp);
		clock_gettime(CLOCK_MONOTONIC, &tend);
		free(tmp);
	}
	time = (tend.tv_sec-tstart.tv_sec) + (tend.tv_nsec-tstart.tv_nsec)*1.0e-9;
	printf("parallel  prefix sum  time in seconds  : %f\n", time);

	int i;
	for (i = 0; i < n; ++i)
	{
		if (a_s[i]!=a_p[i])
		{
			printf("wrong result.\n");
			break;
		}
	}
	if (i == n)
	{
		printf("correct result.\n");
	}

	free(a_s);
	free(a_p);

	return 0;
}
