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
		fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
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




