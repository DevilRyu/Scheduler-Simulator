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
		
		fprintf(stderr, "Ejecución incorrecta del programa schedgen\n");
		fprintf(stderr, "Uso:\n ./build/schedgen d f1 f2 filename\n ");
		fprintf(stderr, "Opciones:\n");
		fprintf(stderr," \td: Número de datos de prueba debe generar.\n");
		fprintf(stderr," \tf: Frecuencia con la que los datos de prueba(procesos) arribaran\n");
		fprintf(stderr," \tf2: Nombre del archivo que contendrá los datos de prueba\n");
		exit(-1);
	}else{
		//validaciones para tipos de argumento valido
		int numGenerados = 0;
		float pa = 0;
		float pb = 0;
		if(sscanf(argv[1], "%d",&numGenerados) != 1) {
    			
			fprintf(stderr, "Ejecución incorrecta del programa schedgen\n");
			fprintf(stderr, "Uso:\n ./build/schedgen d f1 f2 filename\n ");
			fprintf(stderr, "Opciones:\n");
			fprintf(stderr," \td: Número de datos de prueba debe generar.\n");
			fprintf(stderr," \tf: Frecuencia con la que los datos de prueba(procesos) arribaran\n");
			fprintf(stderr," \tf2: Nombre del archivo que contendrá los datos de prueba\n");
			exit(-1);

		}
		if(sscanf(argv[2], "%f",&pa) != 1) {
    			
			fprintf(stderr, "Ejecución incorrecta del programa schedgen\n");
			fprintf(stderr, "Uso:\n ./build/schedgen d f1 f2 filename\n ");
			fprintf(stderr, "Opciones:\n");
			fprintf(stderr," \td: Número de datos de prueba debe generar.\n");
			fprintf(stderr," \tf: Frecuencia con la que los datos de prueba(procesos) arribaran\n");
			fprintf(stderr," \tf2: Nombre del archivo que contendrá los datos de prueba\n");
			exit(-1);			
			
		}
		if(sscanf(argv[3], "%f",&pb) != 1) {

			fprintf(stderr, "Ejecución incorrecta del programa schedgen\n");
			fprintf(stderr, "Uso:\n ./build/schedgen d f1 f2 filename\n ");
			fprintf(stderr, "Opciones:\n");
			fprintf(stderr," \td: Número de datos de prueba debe generar.\n");
			fprintf(stderr," \tf: Frecuencia con la que los datos de prueba(procesos) arribaran\n");
			fprintf(stderr," \tf2: Nombre del archivo que contendrá los datos de prueba\n");
			exit(-1);

		}
		srand((long)time(NULL)); 
		
		FILE * fp;
		char filename[50];
		strcpy(filename, argv[4]);
		//strcat(filename, ".txt");
		char folder[50] = "result/";
		strcat(folder,filename);
		fp = fopen (folder,"w");
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
