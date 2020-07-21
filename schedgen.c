#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

int main(int argc, char **argv)
{
	    	
	if(argc!=5){
		
		fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
		exit(-1);
	}else{
		//validaciones para tipos de argumento valido
		int numGenerados = 0;
		float pa = 0;
		float pb = 0;
		if(sscanf(argv[1], "%d",&numGenerados) != 1) {
    			
			printf("ERROR");

		}
		if(sscanf(argv[2], "%f",&pa) != 1) {
    			
			printf("ERROR");			
			
		}
		if(sscanf(argv[3], "%f",&pb) != 1) {

			printf("ERROR");

		}
		srand((long)time(NULL)); 
		
		FILE * fp;
		char filename[50];
		strcpy(filename, argv[4]);
		//strcat(filename, ".txt");
		fp = fopen (filename,"w");
		int ta=0;
		int i = 0;
		

		while(i < numGenerados){
			float n =  (rand()/(double)RAND_MAX);
			if(n<pa){
				
				int tb=1;
				char b = 1;
				while(b){
					//srand((long)time(NULL)); 
					float m = (rand()/(double)RAND_MAX);
					if(m<pb){
						fprintf (fp, "%d %d\n",ta,tb);
						b = 0;
						i++;
					}else{
						tb++;
					}
				}
			}else{
				ta++;
			}
			
			
		}
		fclose (fp);
		
		
		
	}
	printf("Archivo generado exitosamente\n");

}
