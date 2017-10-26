#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAXLINE 200
#define NBINS 16

float messageComplexity(char *s);

int main(int argc, char ** argv){
	struct timespec tstart,tend; 
 	float timer;
	int hist[NBINS] = {0};
	char **tweets;
	char *buf;
	float delta = 1.0/NBINS;
	if(argc < 3){
		printf("Usage: %s filename numlines numChars\n", argv[0]);
		return 1;
	}
	FILE *f = fopen(argv[1], "r");
	int numLines = strtol(argv[2], NULL, 10);
	int numChars = strtol(argv[3], NULL, 10);
	tweets = malloc(numLines*sizeof(char*));
	buf = malloc(numChars*sizeof(char));
	if(!tweets || !buf){
		fprintf(stderr, "couldn't malloc\n");
		return 1;
	}
	int lineNum = 0;
	char *line = buf;
	while(fgets(line, MAXLINE, f) != NULL){
		line[strlen(line)-1] = '\0'; //remove '\n'
		//skip first two tab-delimited fields
		char *sep = "\t";
		char *tok = strtok(line, sep);
		tok = strtok(NULL, sep);
		tok = strtok(NULL, sep);
		if(tok == NULL)
			continue;
		tweets[lineNum++] = tok;
		line = tok + strlen(tok) + 1; //skip rest of line
	}
    clock_gettime(CLOCK_MONOTONIC, &tstart);
	//parallel here
    #pragma omp parallel 
    {
        #pragma omp for nowait 
        for(int i=0; i < lineNum; i++){
            float c = messageComplexity(tweets[i]);
            int i = c==1.0?NBINS-1:c/delta;
            #pragma omp critical
            {
                hist[i]++;
            }
        }
    }
	clock_gettime(CLOCK_MONOTONIC, &tend);
  	timer = (tend.tv_sec-tstart.tv_sec) +
        	(tend.tv_nsec-tstart.tv_nsec)*1.0e-9;
	for(int i=0; i< NBINS; i++)
		printf("%d ", hist[i]);
	printf("\n");
	printf("wallclock time = %f\n", timer);

	return 0;
}

float messageComplexity(char *s){
	int letter[26] = {0};
	while(*s){
		char c = tolower(*s);
		if('a' <= c && c <= 'z')
			letter[c-'a']++;
		s++;
	}
	int count = 0;
	for(int i=0; i < 26; i++){
		count += letter[i] > 0? 1: 0;
	}
	return (float)count/26;
}
