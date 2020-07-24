#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/queue.h> 
#include "motor.h"


int main(int argc, char **argv)
{
	
	if(argc<3||argc>5){
		fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
		exit(-1);
	}else{
		if(argc == 4){
			int quantum = 0 ;
			
			if(!(strcmp(argv[2],"rr") == 0) ){
				fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
				exit(-1);
			}else{
				if(sscanf(argv[3], "%d",&quantum) != 1) {
    					fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
					exit(-1);
				}

				RR(argv[1],quantum);
			}
		}
		else{
			if(!(strcmp(argv[2],"fcfs") == 0) && !(strcmp(argv[2],"sjf") == 0) ){
				fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
				exit(-1);
			}else{
				
				if(strcmp(argv[2],"fcfs") == 0){

					FCFS(argv[1]);							
					
				}else if(strcmp(argv[2],"sjf") == 0){

					SJF(argv[1]);	
				}
			}
			
		}
	
	}
	
    
	exit(0);
}



