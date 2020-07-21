#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/queue.h> 

#define NTHREADS 3
#define MAXPROCESS

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

// Priority queu for SJF algorithm 
typedef struct node { 

    int priority; 
    struct Proceso *proceso;
    struct node* next; 
  
} Node; 
  

Node* newNode(struct Proceso *proceso, int p) 
{ 
    Node* temp = (Node*)malloc(sizeof(Node)); 
    temp->proceso = proceso; 
    temp->priority = p; 
    temp->next = NULL; 
  
    return temp; 
} 
  

struct Proceso*	peek(Node** head) 
{ 
    return (*head)->proceso; 
} 
  

void pop(Node** head) 
{ 
    Node* temp = *head; 
    (*head) = (*head)->next; 
    free(temp); 
} 
  

void push(Node** head, struct Proceso *proceso, int p) 
{ 
    Node* start = (*head); 

    Node* temp = newNode(proceso, p); 
    if ((*head)->priority > p) { 

        temp->next = *head; 
        (*head) = temp; 
    } 
    else { 

        while (start->next != NULL && 
               start->next->priority < p) { 
            start = start->next; 
        } 
  
        temp->next = start->next; 
        start->next = temp; 
    } 
} 
  

int isEmpty(Node** head) 
{ 
    return (*head) == NULL; 
} 


struct Proceso {
	int id;
	int ta;
	int tb;
	int tbf;
	int tw ;
	int tr0;
	int trf;
	char nuevo;
	STAILQ_ENTRY(Proceso) pointers;
} ;

STAILQ_HEAD(colaL, Proceso);
struct colaL colaListos;

STAILQ_HEAD(colaI, Proceso);
struct colaI colaInterrumpidos;

void leerArchivo();
static int tiempo = -1;
char done;

struct Proceso procesos[5000];    
long numProcesos = 0;
FILE *fp;
int bufferLength = 255;
float tiempoTotalRetorno = 0;
float tiempoPromedioRetPorProceso = 0;
float tiempoTotalEspera = 0;
int ubicacionP = 0;
int numProcesosNuevos = 0;
int numProcesosInterrumpidos = 0;
Node* colaListosSJF = NULL; 
Node* colaInterrumpidosSJF = NULL; 

void *encolador(void *arg)
{

		for (;;){
			pthread_mutex_lock(&mtx);
			pthread_cond_wait(&cond, &mtx);
			if(procesos[ubicacionP].ta == tiempo){
				while(procesos[ubicacionP].ta == tiempo){

					STAILQ_INSERT_TAIL(&colaListos, &procesos[ubicacionP], pointers);
					ubicacionP++;
					numProcesosNuevos++;
				
				}
			}
			pthread_mutex_unlock(&mtx);
			pthread_cond_signal(&cond2);
			done = (ubicacionP == numProcesos && STAILQ_EMPTY(&colaListos) && STAILQ_EMPTY(&colaInterrumpidos));
			if(done){
				break;			
			}

		}
	
    return NULL;
}

void *encoladorSJF(void *arg)
{

		for (;;){
			pthread_mutex_lock(&mtx);
			pthread_cond_wait(&cond, &mtx);
			if(procesos[ubicacionP].ta == tiempo){
				while(procesos[ubicacionP].ta == tiempo){
					if(ubicacionP == 0){
						colaListosSJF = newNode(&procesos[ubicacionP],procesos[ubicacionP].tbf);
						/*colaInterrumpidosSJF = newNode(&procesos[ubicacionP],procesos[ubicacionP].tb);
						pop(&colaInterrumpidosSJF);*/
						ubicacionP++;
						numProcesosNuevos++;
					}else{
						if(!isEmpty(&colaListosSJF)){
							if(peek(&colaListosSJF)->tbf > procesos[ubicacionP].tbf){
							
								struct Proceso *proceso = peek(&colaListosSJF);
								pop(&colaListosSJF);
								numProcesosNuevos--;
								if(isEmpty(&colaInterrumpidosSJF)){
									colaInterrumpidosSJF = newNode(proceso,proceso->tbf);
								}else{
									push(&colaInterrumpidosSJF,proceso,proceso->tbf);
								}
								printf("%d: runs %d-%d\n",proceso->id,proceso->tr0,proceso->trf);
								numProcesosInterrumpidos++;
								if(isEmpty(&colaListosSJF)){
									colaListosSJF = newNode(&procesos[ubicacionP],procesos[ubicacionP].tbf);
								}else{
									push(&colaListosSJF,&procesos[ubicacionP],procesos[ubicacionP].tbf);
								}
								
								ubicacionP++;
								numProcesosNuevos++;
						
							}else{
								if(isEmpty(&colaListosSJF)){
									colaListosSJF = newNode(&procesos[ubicacionP],procesos[ubicacionP].tbf);
								}else{
									push(&colaListosSJF,&procesos[ubicacionP],procesos[ubicacionP].tbf);
								}
								
								ubicacionP++;
								numProcesosNuevos++;
								
							}
						}
						else{
							if(isEmpty(&colaListosSJF)){
								colaListosSJF = newNode(&procesos[ubicacionP],procesos[ubicacionP].tbf);
							}else{
								push(&colaListosSJF,&procesos[ubicacionP],procesos[ubicacionP].tbf);
							}
							
							ubicacionP++;
							numProcesosNuevos++;
							
						}
						
					}
					
				}
			}
			pthread_mutex_unlock(&mtx);
			pthread_cond_signal(&cond2);
			done = (ubicacionP == numProcesos && isEmpty(&colaListosSJF) && isEmpty(&colaInterrumpidosSJF));
			if(done){
				break;			
			}

		}
	
    return NULL;
}



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

				char filename[bufferLength] ;
				strcpy(filename,argv[1]);
				leerArchivo(filename);
				fprintf(stdout,"RR\n");
				printf("\n");
				pthread_t tid;
				//rr algorithm
				
				STAILQ_INIT(&colaListos);

				STAILQ_INIT(&colaInterrumpidos);
				
				pthread_create(&tid, NULL, encolador,NULL);
				char inicio = 1;
				while(!(ubicacionP == numProcesos && STAILQ_EMPTY(&colaListos) && STAILQ_EMPTY(&colaInterrumpidos) )){
					sleep(1); 
					pthread_mutex_lock(&mtx);
					tiempo++; 
					pthread_mutex_unlock(&mtx);
					pthread_cond_signal(&cond);
					pthread_mutex_lock(&mtx);
					pthread_cond_wait(&cond2, &mtx);
					if(STAILQ_EMPTY(&colaListos) && !STAILQ_EMPTY(&colaInterrumpidos)){
						struct Proceso *proceso2;
						proceso2 = STAILQ_FIRST(&colaInterrumpidos);
						STAILQ_REMOVE_HEAD(&colaInterrumpidos, pointers);
						numProcesosInterrumpidos --;
						STAILQ_INSERT_TAIL(&colaListos, proceso2, pointers);
						numProcesosNuevos++;
					}
					
					if(!STAILQ_EMPTY(&colaListos)){

						struct Proceso *proceso;
						proceso = STAILQ_FIRST(&colaListos);
						if(inicio ){	
							proceso->tr0 = tiempo;
							inicio =0;
						}
						proceso->tbf--;
						quantum--;
						if(quantum == 0 || proceso->tbf ==0 ){
							sscanf(argv[3], "%d",&quantum);
							inicio =1;
							proceso->trf=tiempo+1;
							if(proceso->tbf ==0){
								
								printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempo+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
								tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
								tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
								tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);
								STAILQ_REMOVE_HEAD(&colaListos, pointers);
								numProcesosNuevos--;	
							}else{
								printf("%d: runs %d-%d\n",proceso->id,proceso->tr0,proceso->trf);
								STAILQ_REMOVE_HEAD(&colaListos, pointers);
								numProcesosNuevos--;
								STAILQ_INSERT_TAIL(&colaInterrumpidos, proceso, pointers);
								numProcesosInterrumpidos ++;	
							}
						}
						
						
						
					}
					pthread_mutex_unlock(&mtx);
				}
				printf("\n");
				printf("Average turnaround time	= %.2f\n",tiempoTotalRetorno/(numProcesos+0.0));
				printf("Average normalized turnaround time = %.2f\n",(tiempoPromedioRetPorProceso/(numProcesos+0.0)));
				printf("Average waiting time = %.2f\n",tiempoTotalEspera/(numProcesos+0.0));
			}
		}
		else{
			if(!(strcmp(argv[2],"fcfs") == 0) && !(strcmp(argv[2],"sjf") == 0) ){
				fprintf(stderr, "El programa funciona sin argumentos o con un solo argumento \n");
				exit(-1);
			}else{
				
				if(strcmp(argv[2],"fcfs") == 0){

					char filename[bufferLength] ;
					strcpy(filename,argv[1]);
					leerArchivo(filename);
					fprintf(stdout,"FCFS\n");
					printf("\n");
					pthread_t tid;
					//fcfs algorithm

					STAILQ_INIT(&colaListos);

					STAILQ_INIT(&colaInterrumpidos);
					
					pthread_create(&tid, NULL, encolador,NULL);
					char inicio = 1;
					while(!(ubicacionP == numProcesos && STAILQ_EMPTY(&colaListos) && STAILQ_EMPTY(&colaInterrumpidos) )){
						sleep(1); 
						pthread_mutex_lock(&mtx);
						tiempo++; 
						pthread_mutex_unlock(&mtx);
						pthread_cond_signal(&cond);
						pthread_mutex_lock(&mtx);
						pthread_cond_wait(&cond2, &mtx);
						if(!STAILQ_EMPTY(&colaListos)){

							struct Proceso *proceso;
							proceso = STAILQ_FIRST(&colaListos);
							if(inicio){	
								proceso->tr0 = tiempo;
								inicio =0;
							}
							proceso->tbf--;
							if(proceso->tbf ==0){
								inicio =1;
								proceso->trf=tiempo+1;
								printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempo+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
								tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
								tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
								tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);
								STAILQ_REMOVE_HEAD(&colaListos, pointers);
								numProcesosNuevos --;
							}
							
							
							
						}
						pthread_mutex_unlock(&mtx);
					}
					printf("\n");
					printf("Average turnaround time	= %.2f\n",tiempoTotalRetorno/(numProcesos+0.0));
					printf("Average normalized turnaround time = %.2f\n",(tiempoPromedioRetPorProceso/(numProcesos+0.0)));
					printf("Average waiting time = %.2f\n",tiempoTotalEspera/(numProcesos+0.0));							
					
				}else if(strcmp(argv[2],"sjf") == 0){

					char filename[bufferLength] ;
					strcpy(filename,argv[1]);
					leerArchivo(filename);
					fprintf(stdout,"SJF\n");
					printf("\n");
					pthread_t tid;
					//sjf algorithm
					
					pthread_create(&tid, NULL, encoladorSJF,NULL);
					char inicio = 1;
					while(!(ubicacionP == numProcesos && isEmpty(&colaListosSJF) && isEmpty(&colaInterrumpidosSJF))){
						
						sleep(1); 
						pthread_mutex_lock(&mtx);
						tiempo++; 
						pthread_mutex_unlock(&mtx);
						pthread_cond_signal(&cond);
						pthread_mutex_lock(&mtx);
						pthread_cond_wait(&cond2, &mtx);
						
						//printf("Aqui\n");
						
						/*if(isEmpty(&colaListosSJF) && !isEmpty(&colaInterrumpidosSJF)){
							struct Proceso *proceso2;
							proceso2 = peek(&colaInterrumpidosSJF);
							pop(&colaInterrumpidosSJF);
							numProcesosInterrumpidos --;
							push(&colaListosSJF, proceso2, proceso2->tb);
							numProcesosNuevos++;
						}*/
						
						if(!isEmpty(&colaListosSJF)||!isEmpty(&colaInterrumpidosSJF)){
							
							struct Proceso *proceso;
							
							if(!isEmpty(&colaListosSJF)&&!isEmpty(&colaInterrumpidosSJF)){
								struct Proceso *procesot1;
								procesot1 = peek(&colaListosSJF);
								struct Proceso *procesot2;
								procesot2 = peek(&colaInterrumpidosSJF);
								if(procesot1->tbf < procesot2->tbf){
									proceso=procesot1;
								}else{
									proceso=procesot2;
									pop(&colaInterrumpidosSJF);
									numProcesosInterrumpidos --;
									push(&colaListosSJF, procesot2, procesot2->tbf);
									numProcesosNuevos++;
								}							
							}
							
							if(isEmpty(&colaListosSJF)&&!isEmpty(&colaInterrumpidosSJF)){
								struct Proceso *proceso2;
								proceso2 = peek(&colaInterrumpidosSJF);
								proceso = proceso2;
								pop(&colaInterrumpidosSJF);
								numProcesosInterrumpidos --;
								colaListosSJF = newNode(proceso2,proceso2->tbf);
								numProcesosNuevos++;
							}
							
							if(!isEmpty(&colaListosSJF)&&isEmpty(&colaInterrumpidosSJF)){
								proceso = peek(&colaListosSJF);
							}
							if(inicio || proceso->tbf == proceso->tb){	
								proceso->tr0 = tiempo;
								inicio =0;
							}
							proceso->tbf--;
							if( proceso->tbf ==0 ){
								inicio =1;
								proceso->trf=tiempo+1;
								//if(proceso->tbf ==0){
								printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempo+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
								
								tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
								tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
								tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);
								pop(&colaListosSJF);
								
								numProcesosNuevos--;
								/*}else{
									printf("%d: runs %d-%d\n",proceso->id,proceso->tr0,proceso->trf);
									STAILQ_REMOVE_HEAD(&colaListos, pointers);
									numProcesosNuevos--;
									STAILQ_INSERT_TAIL(&colaInterrumpidos, proceso, pointers);
									numProcesosInterrumpidos ++;	
								}*/
							}
							
							
							
							
						}
						
						pthread_mutex_unlock(&mtx);
					}
					printf("\n");
					printf("Average turnaround time	= %.2f\n",tiempoTotalRetorno/(numProcesos+0.0));
					printf("Average normalized turnaround time = %.2f\n",(tiempoPromedioRetPorProceso/(numProcesos+0.0)));
					printf("Average waiting time = %.2f\n",tiempoTotalEspera/(numProcesos+0.0));
				}
			}
			
		}
	
	}
	
    
	exit(0);
}

void leerArchivo(char *filename){

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
	      perror("Error while opening the file.\n");
	      exit(EXIT_FAILURE);
	}
	char buffer[bufferLength];
	while(fgets(buffer, bufferLength, fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		char * token = strtok(buffer, " ");
		
		struct Proceso proceso;
		proceso.id = numProcesos+1;
		char primero = 1;

    		while( token != NULL ) {
			char m[100];
			strcpy(m,token);
			if(primero){
				proceso.ta = atoi(token);
				primero = 0;
			}else{
				proceso.tb = atoi(token);
			}
			
	      		token = strtok(NULL, " ");
			
	   	}
		procesos[numProcesos].id = proceso.id;
		procesos[numProcesos].ta =  proceso.ta;
		procesos[numProcesos].tb = proceso.tb;
		procesos[numProcesos].tbf = proceso.tb;
		procesos[numProcesos].tw = 0;
		procesos[numProcesos].tr0 = 0;
		procesos[numProcesos].trf = 0;
		procesos[numProcesos].nuevo = 1;
		numProcesos ++ ;
	}

}


