#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int isPrime(int n);

int main(int argc, char *argv[]){
	if(argc < 2) return 1;
	int size = strtol(argv[1], NULL, 10);
	int prime = 0;
	int count = 0;
	for(int i=3; i<size; i+=2){
		int new = isPrime(i);
		count += prime&new;
		prime = new;
	}
	printf("num = %d\n", count);
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
