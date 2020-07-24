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

STAILQ_HEAD(colaLFCFS, Proceso);
struct colaLFCFS colaListosFCFS;

STAILQ_HEAD(colaIFCFS, Proceso);
struct colaIFCFS colaInterrumpidosFCFS;

STAILQ_HEAD(colaLRR1, Proceso);
struct colaLRR1 colaListosRR1;

STAILQ_HEAD(colaIRR1, Proceso);
struct colaIRR1 colaInterrumpidosRR1;

STAILQ_HEAD(colaLRR4, Proceso);
struct colaLRR4 colaListosRR4;

STAILQ_HEAD(colaIRR4, Proceso);
struct colaIRR4 colaInterrumpidosRR4;

void leerArchivo();
void escribirTiemposRetorno();
void escribirTiemposEspera();
void escribirTiemposRetornoNormalizados();

static int tiempo = -1;
char done;

static pthread_mutex_t mtxFCFS = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condFCFS = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2FCFS = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t mtxRR1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condRR1 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2RR1 = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t mtxRR4 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condRR4 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2RR4 = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t mtxSJF = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condSJF = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2SJF = PTHREAD_COND_INITIALIZER;


static int tiempoFCFS = -1;
static int tiempoRR1 = -1;
static int tiempoRR4 = -1;
static int tiempoSJF = -1;

char doneFCFS;
char doneRR1;
char doneRR4;
char doneSJF;

pthread_t tid4;
pthread_t tid2;
pthread_t tid1;

struct Proceso procesos[5000];    
long numProcesos = 0;
FILE *fp;
int bufferLength = 255;

float tiempoTotalRetorno = 0;
float tiempoPromedioRetPorProceso = 0;
float tiempoTotalEspera = 0;

int ubicacionPFCFS = 0;
int ubicacionPRR1 = 0;
int ubicacionPRR4 = 0;
int ubicacionPSJF = 0;

int numProcesosNuevosFCFS = 0;
int numProcesosInterrumpidosFCFS = 0;

int numProcesosNuevosRR1 = 0;
int numProcesosInterrumpidosRR1 = 0;

int numProcesosNuevosRR4 = 0;
int numProcesosInterrumpidosRR4 = 0;

int numProcesosNuevosSJF = 0;
int numProcesosInterrumpidosSJF = 0;

 
float tiemposRetorno[250];
float tiemposRetornoNormalizados[250];
float tiemposEspera[250];

int ubicacionP = 0;
int numProcesosNuevos = 0;
int numProcesosInterrumpidos = 0;

Node* colaListosSJF = NULL; 
Node* colaInterrumpidosSJF = NULL; 
pthread_t tid3;
char timeBurst[250];

void *encolador(void *arg)
{

		for (;;){
			
			pthread_mutex_lock(&mtx);
			pthread_cond_wait(&cond, &mtx);
			
			if(procesos[ubicacionP].ta == tiempo){
				
				while(procesos[ubicacionP].ta == tiempo && ubicacionP < numProcesos){
					
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

void *encoladorFCFS(void *arg)
{

		for (;;){
			pthread_mutex_lock(&mtxFCFS);
			pthread_cond_wait(&condFCFS, &mtxFCFS);
			if(procesos[ubicacionPFCFS].ta == tiempoFCFS){
				while(procesos[ubicacionPFCFS].ta == tiempoFCFS && ubicacionPFCFS < numProcesos){

					STAILQ_INSERT_TAIL(&colaListosFCFS, &procesos[ubicacionPFCFS], pointers);
					ubicacionPFCFS++;
					numProcesosNuevosFCFS++;
				
				}
			}
			pthread_mutex_unlock(&mtxFCFS);
			pthread_cond_signal(&cond2FCFS);
			doneFCFS = (ubicacionPFCFS == numProcesos && STAILQ_EMPTY(&colaListosFCFS) && STAILQ_EMPTY(&colaInterrumpidosFCFS));
			if(doneFCFS){
				break;			
			}

		}
	
    return NULL;
}

void *encoladorRR1(void *arg)
{

		for (;;){
			pthread_mutex_lock(&mtxRR1);
			pthread_cond_wait(&condRR1, &mtxRR1);
			if(procesos[ubicacionPRR1].ta == tiempoRR1){
				while(procesos[ubicacionPRR1].ta == tiempoRR1 && ubicacionPRR1 < numProcesos){

					STAILQ_INSERT_TAIL(&colaListosRR1, &procesos[ubicacionPRR1], pointers);
					ubicacionPRR1++;
					numProcesosNuevosRR1++;
				
				}
			}
			pthread_mutex_unlock(&mtxRR1);
			pthread_cond_signal(&cond2RR1);
			doneRR1 = (ubicacionPRR1 == numProcesos && STAILQ_EMPTY(&colaListosRR1) && STAILQ_EMPTY(&colaInterrumpidosRR1));
			if(doneRR1){
				break;			
			}

		}
	
    return NULL;
}
void *encoladorRR4(void *arg)
{

		for (;;){
			
			pthread_mutex_lock(&mtxRR4);
			pthread_cond_wait(&condRR4, &mtxRR4);
			if(procesos[ubicacionPRR4].ta == tiempoRR4){
				while(procesos[ubicacionPRR4].ta == tiempoRR4 && ubicacionPRR4 < numProcesos ){

					STAILQ_INSERT_TAIL(&colaListosRR4, &procesos[ubicacionPRR4], pointers);
					ubicacionPRR4++;
					numProcesosNuevosRR4++;
				
				}
			}
			pthread_mutex_unlock(&mtxRR4);
			pthread_cond_signal(&cond2RR4);
			doneRR4 = (ubicacionPRR4 == numProcesos && STAILQ_EMPTY(&colaListosRR4) && STAILQ_EMPTY(&colaInterrumpidosRR4));
			if(doneRR4){
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
				
				while(procesos[ubicacionP].ta == tiempo && ubicacionP < numProcesos){
					
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
void *encoladorSJF2(void *arg)
{

		for (;;){
			pthread_mutex_lock(&mtxSJF);
			pthread_cond_wait(&condSJF, &mtxSJF);
			if(procesos[ubicacionPSJF].ta == tiempoSJF ){
				while(procesos[ubicacionPSJF].ta == tiempoSJF && ubicacionPSJF < numProcesos){
					if(ubicacionPSJF == 0){
						colaListosSJF = newNode(&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
						/*colaInterrumpidosSJF = newNode(&procesos[ubicacionP],procesos[ubicacionP].tb);
						pop(&colaInterrumpidosSJF);*/
						ubicacionPSJF++;
						numProcesosNuevosSJF++;
					}else{
						if(!isEmpty(&colaListosSJF)){
							if(peek(&colaListosSJF)->tbf > procesos[ubicacionPSJF].tbf){
							
								struct Proceso *proceso = peek(&colaListosSJF);
								pop(&colaListosSJF);
								numProcesosNuevosSJF--;
								if(isEmpty(&colaInterrumpidosSJF)){
									colaInterrumpidosSJF = newNode(proceso,proceso->tbf);
								}else{
									push(&colaInterrumpidosSJF,proceso,proceso->tbf);
								}
								printf("procesando sjf...\n");
								numProcesosInterrumpidosSJF++;
								if(isEmpty(&colaListosSJF)){
									colaListosSJF = newNode(&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
								}else{
									push(&colaListosSJF,&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
								}
								
								ubicacionPSJF++;
								numProcesosNuevosSJF++;
						
							}else{
								if(isEmpty(&colaListosSJF)){
									colaListosSJF = newNode(&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
								}else{
									push(&colaListosSJF,&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
								}
								
								ubicacionPSJF++;
								numProcesosNuevosSJF++;
								
							}
						}
						else{
							if(isEmpty(&colaListosSJF)){
								colaListosSJF = newNode(&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
							}else{
								push(&colaListosSJF,&procesos[ubicacionPSJF],procesos[ubicacionPSJF].tbf);
							}
							
							ubicacionPSJF++;
							numProcesosNuevosSJF++;
							
						}
						
					}
					
				}
			}
			pthread_mutex_unlock(&mtxSJF);
			pthread_cond_signal(&cond2SJF);
			doneSJF = (ubicacionPSJF == numProcesos && isEmpty(&colaListosSJF) && isEmpty(&colaInterrumpidosSJF));
			if(doneSJF){
				break;			
			}

		}
	
    return NULL;
}

void RR(char *arg, int arg2){
	int quantum = 0 ;
	char filename[bufferLength] ;
	strcpy(filename,arg);
	char folder[50] = "result/";
	strcat(folder, filename);
	leerArchivo(folder);
	fprintf(stdout,"RR\n");
	printf("\n");
	int num = arg2;
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
				quantum = num;
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

void FCFS(char *arg){
	char filename[bufferLength] ;
	strcpy(filename,arg);
	char folder[50] = "result/";
	strcat(folder, filename);
	leerArchivo(folder);
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
}

void SJF(char *arg){
	char filename[bufferLength] ;
	strcpy(filename,arg);
	char folder[50] = "result/";
	strcat(folder, filename);
	leerArchivo(folder);
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
				printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempo+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
				
				tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
				tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
				tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);
				pop(&colaListosSJF);
				
				numProcesosNuevos--;
			
			
			}
			
		}
		
		pthread_mutex_unlock(&mtx);
	}
	printf("\n");
	printf("Average turnaround time	= %.2f\n",tiempoTotalRetorno/(numProcesos+0.0));
	printf("Average normalized turnaround time = %.2f\n",(tiempoPromedioRetPorProceso/(numProcesos+0.0)));
	printf("Average waiting time = %.2f\n",tiempoTotalEspera/(numProcesos+0.0));
}

void *simulacionFCFS(void *arg)
{
	pthread_t tid;
	//fcfs algorithm

	STAILQ_INIT(&colaListosFCFS);

	STAILQ_INIT(&colaInterrumpidosFCFS);
	
	pthread_create(&tid, NULL,encoladorFCFS,NULL);
	char inicio = 1;
	while(!(ubicacionPFCFS == numProcesos && STAILQ_EMPTY(&colaListosFCFS) && STAILQ_EMPTY(&colaInterrumpidosFCFS) )){
		sleep(1);  
		pthread_mutex_lock(&mtxFCFS);
		tiempoFCFS++; 
		pthread_mutex_unlock(&mtxFCFS);
		pthread_cond_signal(&condFCFS);
		pthread_mutex_lock(&mtxFCFS);
		pthread_cond_wait(&cond2FCFS, &mtxFCFS);
		if(!STAILQ_EMPTY(&colaListosFCFS)){

			struct Proceso *proceso;
			proceso = STAILQ_FIRST(&colaListosFCFS);
			if(inicio){	
				proceso->tr0 = tiempoFCFS;
				inicio =0;
			}
			proceso->tbf--;
			if(proceso->tbf ==0){
				inicio =1;
				proceso->trf=tiempoFCFS+1;
				printf("procesando fcfs...\n");
				//printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempoFCFS+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
				tiemposRetorno[proceso->tb] = tiemposRetorno[proceso->tb]+proceso->trf-proceso->ta;
				tiemposRetornoNormalizados[proceso->tb] = tiemposRetornoNormalizados[proceso->tb]+(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
				tiemposEspera[proceso->tb] = tiemposEspera[proceso->tb] +((proceso->trf-proceso->ta)-proceso->tb);
				/*tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
				tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
				tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);*/
				STAILQ_REMOVE_HEAD(&colaListosFCFS, pointers);
				numProcesosNuevosFCFS --;
			}
			
			
			
		}
		pthread_mutex_unlock(&mtxFCFS);
	}
	printf("procesamiento finalizado (fcfs)\n");
	return NULL;
}

void *simulacionRR1(void *arg)
{

	pthread_t tid;
	//rr algorithm
	int quantum = 1;
	STAILQ_INIT(&colaListosRR1);

	STAILQ_INIT(&colaInterrumpidosRR1);
	
	pthread_create(&tid, NULL, encoladorRR1,NULL);
	char inicio = 1;
	while(!(ubicacionPRR1 == numProcesos && STAILQ_EMPTY(&colaListosRR1) && STAILQ_EMPTY(&colaInterrumpidosRR1) )){
		sleep(1); 
		pthread_mutex_lock(&mtxRR1);
		tiempoRR1++; 
		pthread_mutex_unlock(&mtxRR1);
		pthread_cond_signal(&condRR1);
		pthread_mutex_lock(&mtxRR1);
		pthread_cond_wait(&cond2RR1, &mtxRR1);
		if(STAILQ_EMPTY(&colaListosRR1) && !STAILQ_EMPTY(&colaInterrumpidosRR1)){
			struct Proceso *proceso2;
			proceso2 = STAILQ_FIRST(&colaInterrumpidosRR1);
			STAILQ_REMOVE_HEAD(&colaInterrumpidosRR1, pointers);
			numProcesosInterrumpidosRR1 --;
			STAILQ_INSERT_TAIL(&colaListosRR1, proceso2, pointers);
			numProcesosNuevosRR1++;
		}
		
		if(!STAILQ_EMPTY(&colaListosRR1)){

			struct Proceso *proceso;
			proceso = STAILQ_FIRST(&colaListosRR1);
			if(inicio ){	
				proceso->tr0 = tiempoRR1;
				inicio =0;
			}
			proceso->tbf--;
			quantum--;
			if(quantum == 0 || proceso->tbf ==0 ){
				quantum = 1;
				inicio =1;
				proceso->trf=tiempoRR1+1;
				if(proceso->tbf ==0){
					printf("procesando rr1...\n");
					//printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempoRR1+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
					tiemposRetorno[proceso->tb] = tiemposRetorno[proceso->tb]+proceso->trf-proceso->ta;
					tiemposRetornoNormalizados[proceso->tb] = tiemposRetornoNormalizados[proceso->tb]+(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
					tiemposEspera[proceso->tb] = tiemposEspera[proceso->tb] +((proceso->trf-proceso->ta)-proceso->tb);
					/*tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
					tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
					tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);*/
					STAILQ_REMOVE_HEAD(&colaListosRR1, pointers);
					numProcesosNuevosRR1--;	
				}else{
					printf("procesando rr1...\n");
					//printf("%d: runs %d-%d\n",proceso->id,proceso->tr0,proceso->trf);
					STAILQ_REMOVE_HEAD(&colaListosRR1, pointers);
					numProcesosNuevosRR1--;
					STAILQ_INSERT_TAIL(&colaInterrumpidosRR1, proceso, pointers);
					numProcesosInterrumpidosRR1 ++;	
				}
			}
			
			
			
		}
		pthread_mutex_unlock(&mtxRR1);
	}
	printf("procesamiento finalizado (rr1)\n");
	return NULL;
	
}

void *simulacionRR4(void *arg)
{

	pthread_t tid;
	//rr algorithm
	int quantum = 4;
	STAILQ_INIT(&colaListosRR4);

	STAILQ_INIT(&colaInterrumpidosRR4);
	
	pthread_create(&tid, NULL, encoladorRR4,NULL);
	char inicio = 1;
	while(!(ubicacionPRR4 == numProcesos && STAILQ_EMPTY(&colaListosRR4) && STAILQ_EMPTY(&colaInterrumpidosRR4) )){
		sleep(1);  
		pthread_mutex_lock(&mtxRR4);
		tiempoRR4++; 
		pthread_mutex_unlock(&mtxRR4);
		pthread_cond_signal(&condRR4);
		pthread_mutex_lock(&mtxRR4);
		pthread_cond_wait(&cond2RR4, &mtxRR4);
		if(STAILQ_EMPTY(&colaListosRR4) && !STAILQ_EMPTY(&colaInterrumpidosRR4)){
			struct Proceso *proceso2;
			proceso2 = STAILQ_FIRST(&colaInterrumpidosRR4);
			STAILQ_REMOVE_HEAD(&colaInterrumpidosRR4, pointers);
			numProcesosInterrumpidosRR4 --;
			STAILQ_INSERT_TAIL(&colaListosRR4, proceso2, pointers);
			numProcesosNuevosRR4++;
		}
		
		if(!STAILQ_EMPTY(&colaListosRR4)){

			struct Proceso *proceso;
			proceso = STAILQ_FIRST(&colaListosRR4);
			if(inicio ){	
				proceso->tr0 = tiempoRR4;
				inicio =0;
			}
			proceso->tbf--;
			quantum--;
			if(quantum == 0 || proceso->tbf ==0 ){
				quantum = 4;
				inicio =1;
				proceso->trf=tiempoRR4+1;
				if(proceso->tbf ==0){
					printf("procesando rr4...\n");
					//printf("%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst=%d), wait = %d\n",proceso->id,proceso->tr0,proceso->trf,tiempoRR4+1,proceso->ta, proceso->trf-proceso->ta, proceso->tb,(proceso->trf-proceso->ta)-proceso->tb);
					tiemposRetorno[proceso->tb] = tiemposRetorno[proceso->tb]+proceso->trf-proceso->ta;
					tiemposRetornoNormalizados[proceso->tb] = tiemposRetornoNormalizados[proceso->tb]+(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
					tiemposEspera[proceso->tb] = tiemposEspera[proceso->tb] +((proceso->trf-proceso->ta)-proceso->tb);
					/*tiempoTotalRetorno = tiempoTotalRetorno + proceso->trf-proceso->ta;
					tiempoPromedioRetPorProceso =  tiempoPromedioRetPorProceso +(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
					tiempoTotalEspera = tiempoTotalEspera+((proceso->trf-proceso->ta)-proceso->tb);*/
					STAILQ_REMOVE_HEAD(&colaListosRR4, pointers);
					numProcesosNuevosRR4--;	
				}else{
					printf("procesando rr4...\n");
					//printf("%d: runs %d-%d\n",proceso->id,proceso->tr0,proceso->trf);
					STAILQ_REMOVE_HEAD(&colaListosRR4, pointers);
					numProcesosNuevosRR4--;
					STAILQ_INSERT_TAIL(&colaInterrumpidosRR4, proceso, pointers);
					numProcesosInterrumpidosRR4 ++;	
				}
			}
			
			
			
		}
		pthread_mutex_unlock(&mtxRR4);
	}
	printf("procesamiento finalizado (rr4)\n");
	return NULL;
	
}

void *simulacionSJF(void *arg)
{
	pthread_t tid;
	//sjf algorithm
	
	pthread_create(&tid, NULL, encoladorSJF2,NULL);
	char inicio = 1;
	while(!(ubicacionPSJF == numProcesos && isEmpty(&colaListosSJF) && isEmpty(&colaInterrumpidosSJF))){
		
		sleep(1); 
		pthread_mutex_lock(&mtxSJF);
		tiempoSJF ++; 
		pthread_mutex_unlock(&mtxSJF);
		pthread_cond_signal(&condSJF);
		pthread_mutex_lock(&mtxSJF);
		pthread_cond_wait(&cond2SJF ,&mtxSJF);
		
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
					numProcesosInterrumpidosSJF  --;
					push(&colaListosSJF, procesot2, procesot2->tbf);
					numProcesosNuevosSJF ++;
				}							
			}
			
			if(isEmpty(&colaListosSJF)&&!isEmpty(&colaInterrumpidosSJF)){
				struct Proceso *proceso2;
				proceso2 = peek(&colaInterrumpidosSJF);
				proceso = proceso2;
				pop(&colaInterrumpidosSJF);
				numProcesosInterrumpidosSJF  --;
				colaListosSJF = newNode(proceso2,proceso2->tbf);
				numProcesosNuevosSJF ++;
			}
			
			if(!isEmpty(&colaListosSJF)&&isEmpty(&colaInterrumpidosSJF)){
				proceso = peek(&colaListosSJF);
			}
			if(inicio || proceso->tbf == proceso->tb){	
				proceso->tr0 = tiempoSJF ;
				inicio =0;
			}
			proceso->tbf--;
			if( proceso->tbf ==0 ){
				inicio =1;
				proceso->trf=tiempoSJF +1;
				//if(proceso->tbf ==0){
				printf("procesando sjf...\n");
				
				tiemposRetorno[proceso->tb] = tiemposRetorno[proceso->tb]+proceso->trf-proceso->ta;
				tiemposRetornoNormalizados[proceso->tb] = tiemposRetornoNormalizados[proceso->tb]+(( proceso->trf-proceso->ta)/(proceso->tb+0.00));
				tiemposEspera[proceso->tb] = tiemposEspera[proceso->tb] +((proceso->trf-proceso->ta)-proceso->tb);
				pop(&colaListosSJF);
				
				numProcesosNuevosSJF --;
				/*}else{
					printf("%d: runs %d-%d\n",proceso->id,proceso->tr0,proceso->trf);
					STAILQ_REMOVE_HEAD(&colaListos, pointers);
					numProcesosNuevos--;
					STAILQ_INSERT_TAIL(&colaInterrumpidos, proceso, pointers);
					numProcesosInterrumpidos ++;	
				}*/
			}
			
			
			
			
		}
		
		pthread_mutex_unlock(&mtxSJF );
	}
	printf("procesamiento finalizado (sjf)\n");
	return NULL;	
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

void escribirTiemposRetorno(int numLinea1, char *filenameTemp){
		
	FILE * fPtr;
	FILE * fTemp;
	char path[100] = "schedturns.dat";

	char buffer[1000];
	char newline[1000];
	int count;
	char folder[50] = "result/";
	strcat(folder, path);
	
	fPtr  = fopen(folder, "r");
	fTemp = fopen(filenameTemp, "w"); 

	if (fPtr == NULL || fTemp == NULL)
	{
		
		printf("\nUnable to open file.\n");
		printf("Please check whether file exists and you have read/write privilege.\n");
		exit(EXIT_SUCCESS);
	}

	count = 0;
	while ((fgets(buffer, 1000, fPtr)) != NULL)
	{
		count++;

		/* If current line is line to replace */
		if (count == numLinea1){
			//for y escritura
			for(int i =0;i<250;i++){
				if(timeBurst[i]!=0){
					fprintf(fTemp,"%.2f ",tiemposRetorno[i]/numProcesos+0.0);
				}
				
			}
			fprintf(fTemp,"\n");
			
		}else
		    fputs(buffer, fTemp);
	}


	/* Close all files to release resource */
	fclose(fPtr);
	fclose(fTemp);

	
	fPtr  = fopen(folder, "w");
	fTemp = fopen(filenameTemp, "r"); //darle un temporal a cada uno

	if (fPtr == NULL || fTemp == NULL)
	{
		
		printf("\nUnable to open file.\n");
		printf("Please check whether file exists and you have read/write privilege.\n");
		exit(EXIT_SUCCESS);
	}

	count = 0;
	while ((fgets(buffer, 1000, fTemp)) != NULL)
	{
		  fputs(buffer, fPtr);
	}


	
	fclose(fPtr);
	fclose(fTemp);

	
	remove(filenameTemp);
	return;


}

void escribirTiemposRetornoNormalizados(int numLinea1, char *filenameTemp){
		
	FILE * fPtr;
	FILE * fTemp;
	char path[100] = "schednturns.dat";

	char buffer[1000];
	char newline[1000];
	int count;
	char folder[50] = "result/";
	strcat(folder, path);
	
	fPtr  = fopen(folder, "r");
	fTemp = fopen(filenameTemp, "w"); 

	if (fPtr == NULL || fTemp == NULL)
	{
		
		printf("\nUnable to open file.\n");
		printf("Please check whether file exists and you have read/write privilege.\n");
		exit(EXIT_SUCCESS);
	}

	count = 0;
	while ((fgets(buffer, 1000, fPtr)) != NULL)
	{
		count++;

		/* If current line is line to replace */
		if (count == numLinea1){
			//for y escritura
			for(int i =0;i<250;i++){
				if(timeBurst[i]!=0){
					fprintf(fTemp,"%.2f ",tiemposRetornoNormalizados[i]/numProcesos+0.0);
				}
				
			}
			fprintf(fTemp,"\n");
			
		}
		else
		    fputs(buffer, fTemp);
	}


	/* Close all files to release resource */
	fclose(fPtr);
	fclose(fTemp);

	
	fPtr  = fopen(folder, "w");
	fTemp = fopen(filenameTemp, "r"); //darle un temporal a cada uno

	if (fPtr == NULL || fTemp == NULL)
	{
		
		printf("\nUnable to open file.\n");
		printf("Please check whether file exists and you have read/write privilege.\n");
		exit(EXIT_SUCCESS);
	}

	count = 0;
	while ((fgets(buffer, 1000, fTemp)) != NULL)
	{
		fputs(buffer, fPtr);
		
	}


	/* Close all files to release resource */
	fclose(fPtr);
	fclose(fTemp);

	/* Delete original source file */
	remove(filenameTemp);

	/* Rename temporary file as original file */
	//rename("replace.tmp", path);
	return;

}

void escribirTiemposEspera(int numLinea1, char *filenameTemp){
		
	FILE * fPtr;
	FILE * fTemp;
	char path[100] = "schedwaits.dat";

	char buffer[1000];
	char newline[1000];
	int count;
	char folder[50] = "result/";
	strcat(folder, path);
	
	fPtr  = fopen(folder, "r");
	fTemp = fopen(filenameTemp, "w"); 

	if (fPtr == NULL || fTemp == NULL)
	{
		
		printf("\nUnable to open file.\n");
		printf("Please check whether file exists and you have read/write privilege.\n");
		exit(EXIT_SUCCESS);
	}

	count = 0;
	while ((fgets(buffer, 1000, fPtr)) != NULL)
	{
		count++;

		/* If current line is line to replace */
		if (count == numLinea1){
			//for y escritura
			for(int i =0;i<250;i++){
				if(timeBurst[i]!=0){
					fprintf(fTemp,"%.2f ",tiemposEspera[i]/numProcesos+0.0);
				}
				
			}
			fprintf(fTemp,"\n");
			
		}
		else
		    fputs(buffer, fTemp);
	}


	/* Close all files to release resource */
	fclose(fPtr);
	fclose(fTemp);

	
	fPtr  = fopen(folder, "w");
	fTemp = fopen(filenameTemp, "r"); //darle un temporal a cada uno

	if (fPtr == NULL || fTemp == NULL)
	{
		
		printf("\nUnable to open file.\n");
		printf("Please check whether file exists and you have read/write privilege.\n");
		exit(EXIT_SUCCESS);
	}

	count = 0;
	while ((fgets(buffer, 1000, fTemp)) != NULL)
	{
		fputs(buffer, fPtr);
		
	}


	fclose(fPtr);
	fclose(fTemp);
	remove(filenameTemp);


	return;
}

void seleccionarAlgortmo(int algoritmo){
	switch(algoritmo){
		case(1):
			
			pthread_create(&tid4, NULL, simulacionSJF,NULL);
			pthread_join(tid4, NULL);
			escribirTiemposRetorno(5,"temp4");
			escribirTiemposEspera(5,"temp4");
			escribirTiemposRetornoNormalizados(5,"temp4");
			
			break;
					
		case(2):
			
			pthread_create(&tid3, NULL, simulacionRR4,NULL);
			pthread_join(tid3, NULL);
			
			escribirTiemposRetorno(4,"temp3");
			escribirTiemposEspera(4,"temp3");
			escribirTiemposRetornoNormalizados(4,"temp3");

			break;
		
		case(3):
			
			pthread_create(&tid2, NULL, simulacionRR1,NULL);
			pthread_join(tid2, NULL);
			escribirTiemposRetorno(3,"temp2");
			escribirTiemposEspera(3,"temp2");
			escribirTiemposRetornoNormalizados(3,"temp2");
			break;
		
		case(4):
			
			
			pthread_create(&tid1, NULL, simulacionFCFS,NULL);
			pthread_join(tid1, NULL);
			escribirTiemposRetorno(2,"temp1");
			escribirTiemposEspera(2,"temp1");
			escribirTiemposRetornoNormalizados(2,"temp1");
			break;
		
	}
}

void crearArchivos(){
	FILE *fw;
	fw = fopen("result/schedturns.dat", "w");
	
	for(int i = 0;i<250;i++){

		
		if(timeBurst[i]==1){
			fprintf (fw, "%d ",i);
		}
		
	
	}
	fprintf (fw, "\n");
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fclose(fw);

	fw = fopen("result/schednturns.dat", "w");
	for(int i = 0;i<250;i++){

		
		if(timeBurst[i]==1){
			fprintf (fw, "%d ",i);
		}
		
	
	}
	fprintf (fw, "\n");
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fclose(fw);

	fw = fopen("result/schedwaits.dat", "w");
	for(int i = 0;i<250;i++){

		
		if(timeBurst[i]==1){
			fprintf (fw, "%d ",i);
		}
		
	
	}
	fprintf (fw, "\n");
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fprintf (fw, "%d\n",0);
	fclose(fw);

}

void leerArchivo2(char *filename){

	printf("%s\n",filename);
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
		timeBurst[proceso.tb] = 1;
		procesos[numProcesos].tw = 0;
		procesos[numProcesos].tr0 = 0;
		procesos[numProcesos].trf = 0;
		numProcesos ++ ;
	}

}


