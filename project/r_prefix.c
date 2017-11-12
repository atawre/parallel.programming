#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <time.h>
#include <string.h>

int isPowerOf2(int n){
	while(n){
		if(n & 1)
			break;
		n >>= 1;
	}
	return (1 == n? 1:0);
}

void reverse(int *a, int n){
  int upper_bound = n/2;
  cilk_for(int i = 0; i < upper_bound; i++) {
  	int j = n-i-1;
    a[i] ^= a[j];
    a[j] ^= a[i];
    a[i] ^= a[j];
  }
}

void up(int a[], int n){
	int upper_bound = log2(n);
	if(!isPowerOf2(n)){
		upper_bound += 1;
	}
	for (int k = 0; k < upper_bound; k++)
	{
		int j = exp2(k);
		int stride = 2*j;
		int m = n/stride;
		if(m*stride+j<=n-1){
			m+=1;
		}
		a[0:m:stride] += a[j:m:stride];
	}
}

void down(int a[], int n, int* tmp){
	int upper_bound = log2(n);
	if(!isPowerOf2(n)){
		upper_bound += 1;
	}
    for(int k = upper_bound-1; k >= 0; k--){
        int right  = exp2(k);
        int left   = 0;
        int stride = exp2(k+1);
        int m = n/stride;
        if(m*stride+right<=n-1){
			m+=1;
		}
        tmp[0:m:stride] = a[right:m:stride];
        a[right:m:stride] = a[left:m:stride];
        a[left:m:stride] += tmp[0:m:stride];
    }
}

//Blelloch Prefix Sum
void ex_pref_sum(int a[], int n, int* tmp){
    reverse(a,n);
	up(a,n);
	a[0]=0;
	down(a,n,tmp);
    reverse(a,n);
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


//gcc -fcilkplus reverse_blelloch.c -O2 -o reverse_blelloch -ftree-vectorize -fopt-info-vec-optimized -lrt
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
	for(int i = 0; i < n; ++i){
		a_s[i] = rand() % n;
		a_p[i] = a_s[i];
	}

	//sequential prefix sum for verification and timing
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	prefixOnHost(a_s, n);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	time = (tend.tv_sec-tstart.tv_sec) + (tend.tv_nsec-tstart.tv_nsec)*1.0e-9;
	printf("sequential prefix sum time in seconds: %f\n", time);

	int* tmp = (int*)malloc(sizeof(int)*n);
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	ex_pref_sum(a_p,n,tmp);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	time = (tend.tv_sec-tstart.tv_sec) + (tend.tv_nsec-tstart.tv_nsec)*1.0e-9;
	printf("parallel  prefix sum  time in seconds: %f\n", time);

	int i;
	for(i = 0; i < n; ++i)
		if (a_s[i]!=a_p[i]){
			printf("wrong result.\n");
			break;
		}
	if(i==n)
		printf("correct result.\n");

	free(a_s);
	free(a_p);
	free(tmp);

	return 0;
}
