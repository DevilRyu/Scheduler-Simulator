#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/queue.h> 
#include<sys/wait.h> 
#include "../motor/motor.h"

//char timeBurst[250];

int main(int argc, char **argv)
{
	
	if(argc!=2){
		fprintf(stderr, "Ejecución incorrecta del programa schedstats\n");
		fprintf(stderr, "Uso:\n ./build/schedsim filename algoritmo [q]\n ");
		fprintf(stderr, "Opciones:\n");
		fprintf(stderr," \tfilename: Nombre del archivo que el programa usará para su simulación.\n");
		fprintf(stderr," \talgoritmo: Indica al programa que algoritmo de agendamiento de procesos usar.\n");
		fprintf(stderr," \tq: Parametro obligatorio, si y solo si el algoritmo es RR.Valor entero que indica al programa el valor que tendrá el quantum al momento de realizar la simulación.\n");
		exit(-1);
	}else{
		
		char filename[255] ;
		strcpy(filename,argv[1]);
		char folder[50] = "result/";
		strcat(folder, filename);
		leerArchivo2(folder);
		
		crearArchivos();
		
		
		if(fork()==0){
			if(fork()==0){
				if(fork()==0){
					if(fork()==0){
						seleccionarAlgortmo(1);
						exit(0);
						
					}else{
						seleccionarAlgortmo(2);
						wait(NULL);
						exit(0);
					}
					
				}else{
					seleccionarAlgortmo(3);
					wait(NULL);
					exit(0);
				}
				
			}else{
				seleccionarAlgortmo(4);
				wait(NULL);
				exit(0);	
			}
			
		}else{
			
			wait(NULL);
						
			
		}		

	}
    
	exit(0);
}




