#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/queue.h> 
#include "../motor/motor.h"


int main(int argc, char **argv)
{
	
	if(argc<3||argc>5){
		fprintf(stderr, "Ejecución incorrecta del programa schedsim\n");
		fprintf(stderr, "Uso:\n ./build/schedsim filename algoritmo [q]\n ");
		fprintf(stderr, "Opciones:\n");
		fprintf(stderr," \tfilename: Nombre del archivo que el programa usará para su simulación.\n");
		fprintf(stderr," \talgoritmo: Indica al programa que algoritmo de agendamiento de procesos usar.\n");
		fprintf(stderr," \tq: Parametro obligatorio, si y solo si el algoritmo es RR.Valor entero que indica al programa el valor que tendrá el quantum al momento de realizar la simulación.\n");
		exit(-1);
	}else{
		if(argc == 4){
			int quantum = 0 ;
			
			if(!(strcmp(argv[2],"rr") == 0) ){
				fprintf(stderr, "Ejecución incorrecta del programa schedsim\n");
				fprintf(stderr, "Uso:\n ./build/schedsim filename algoritmo [q]\n ");
				fprintf(stderr, "Opciones:\n");
				fprintf(stderr," \tfilename: Nombre del archivo que el programa usará para su simulación.\n");
				fprintf(stderr," \talgoritmo: Indica al programa que algoritmo de agendamiento de procesos usar.\n");
				fprintf(stderr," \tq: Parametro obligatorio, si y solo si el algoritmo es RR.Valor entero que indica al programa el valor que tendrá el quantum al momento de realizar la simulación.\n");
				exit(-1);
			}else{
				if(sscanf(argv[3], "%d",&quantum) != 1) {
    					fprintf(stderr, "Ejecución incorrecta del programa schedsim\n");
					fprintf(stderr, "Uso:\n ./build/schedsim filename algoritmo [q]\n ");
					fprintf(stderr, "Opciones:\n");
					fprintf(stderr," \tfilename: Nombre del archivo que el programa usará para su simulación.\n");
					fprintf(stderr," \talgoritmo: Indica al programa que algoritmo de agendamiento de procesos usar.\n");
					fprintf(stderr," \tq: Parametro obligatorio, si y solo si el algoritmo es RR.Valor entero que indica al programa el valor que tendrá el quantum al momento de realizar la simulación.\n");
					exit(-1);
				}

				RR(argv[1],quantum);
			}
		}
		else{
			if(!(strcmp(argv[2],"fcfs") == 0) && !(strcmp(argv[2],"sjf") == 0) ){
				fprintf(stderr, "Ejecución incorrecta del programa schedsim\n");
				fprintf(stderr, "Uso:\n ./build/schedsim filename algoritmo [q]\n ");
				fprintf(stderr, "Opciones:\n");
				fprintf(stderr," \tfilename: Nombre del archivo que el programa usará para su simulación.\n");
				fprintf(stderr," \talgoritmo: Indica al programa que algoritmo de agendamiento de procesos usar.\n");
				fprintf(stderr," \tq: Parametro obligatorio, si y solo si el algoritmo es RR.Valor entero que indica al programa el valor que tendrá el quantum al momento de realizar la simulación.\n");
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



