Visitar el repositorio de GitHub dado a continuación para una mejor visualización de la documentación del proyecto:
* https://github.com/DevilRyu/Scheduler-Simulator

# Scheduler-Simulator
Este proyecto consiste en una serie de programas que tienen como fin llevar a cabo una simulación de como es el agendamiento de procesos en un computador, los algoritmos de agendamiento usados fueron: FCFS, SJF, RR. Y los programas implementados son:

* ./schedgen (scheduler generator): programa cuyo fin es generar un archivo con los datos necesarios (time arrive, time burst) para llevar a cabo la simulación.
* ./schedsim (scheduler simulator): es el motor de simulación, este programa realiza todo el proceso de encolamiento e interrución de procesos y muestra como va la ejecución de manera detallada, utiliza programación en paralelo mediante el uso de hilos.
* ./schedstats (scheduler stadistics): usa el motor de antes mencionado pero a diferencia del anterior este programa ejecuta 4 simulaciones al mismo tiempo con los algoritmos de rr1, rr4, sfj, fcfs; por lo que usa programación concurrente mediante procesos y como resultado genera 3 archivos con datos estadísticos para su posterior procesamiento.
* ./ploter.py: este programa toma los archivos del programa anterior para realizar su procesamiento y graficación mediante la librería matplotlib.

Respecto a la organización de directorios:
* src: contiene el código fuente de cada uno de los programas antes mencionados (schedgen.c, schedsim.c, schedstats.c, ploter.py)
* motor: contiene el motor principal para los programas de simulación y generación de resultados (schedsim.c, schedstats.c), esta compuesto por dos archivos: motor.c y motor.h
* resourcesGH: únicamente contiene imagénes generadas por ploter.py que son usadas para mostrar en el repositorio de GitHub.
* result: este directorio es muy importante debido a que es donde irán todos los archivos e imágenes generadas por los distintos programas, por lo tanto se debe evitar eliminarla.
* build: este directorio no se encuntra en la estructura básica de organicación del proyecto debido a que se genera una vez compilemos nuestro poyecto mediante el comando **make** y es donde se alojaran los ejecutables de los archivos .c.

Cabe decir que este proyecto fue realizado en una máquina virtual con sistema operativo **Ubuntu 18.04.4 LTS** con Kernel: **Linux 5.4.0-42-generic** y arquitectura **x86-64**.
En la siguiente sección se explicará más a detalle cada uno de los programas mencionados.
## Modo de uso y compilación
Para poder compilar los programas realizados en C es necesario introducir en la terminal el siguiente comando:                                                       
```
make
```
El cual generara 3 archivos ejecutables correspondientes a sus archivos.c dentro de la carpeta build.
* El primer programa denominado **schedgen** consiste en un generador de datos de pruebas para el simulador de agendamiento de procesos , estos datos son escritos en un archivo   que tiene un formato especifico que es el siguiente:
  ```
  0 2
  0 5 
  5 7
  .
  .
  .
  8 10
  ```
  Donde la primera columna indica el tiempo de arribo de los procesos y la segunda columna su burst time, por lo tanto cada fila corresponde a un único proceso.
  
  Para ejecutar el programa denominado **schedgen** se debe introducir el comando:
  ```
  ./build/schedgen d f1 f2 filename                                                                                                               
  ```
  El cual consta de 4 banderas, las cuales representan lo explicado a continuación:
  * **d:** es un número entero que le indica al programa cuantos datos de prueba debe generar.
  * **f1:** es un número decimal que le indica al programa la frecuencia con la que los datos de prueba(procesos) arribaran.
  * **f2:** es un número decimal que le indica al programa la distribución del burst time de los procesos.
  * **filename:** es una cadena de caracteres que le indica al programa como se llamará el archivo que contenga los datos de prueba.
  Un ejemplo de su uso sería:
  ```
  ./build/schedgen 100 0.5 0.2 schedstimes.dat                                                                                                               
  ```
  
* El segundo programa denominado **schedsim** consiste en un motor de simulación que tiene 3 funcionalidades dependiendo de los parametros de entrada que recibe por consola este simulador, consta de 3 algortimos que son FCFS, RR y SJF apropiativos, el comando para ejecutar sus diferentes funcionalidades es:
  ```
  ./build/schedsim filename algoritmo [q]                                                                                                              
  ```
  Este programa consta principalmente de 2 paramentros obligatorios en cualquier ejecución que son:
  * **filename:** es una cadena de caracteres que le indica al programa que dataset de prueba usará para su simulación.
  * **algoritmo:** es una cadena de caracteres que le indica al programa que algoritmo de agendamiento de procesos usar.
  El tercer párametro es obligatorio, si y solo si, el algoritmo de agendamiento escogido es Round Robin:
  * **q:** es un número entero que le indica al programa el valor que tendrá el quantum al momento de realizar la simulación.
  Un ejemplo por cada uno de las posibilidades de ejecución del programa sería:
   ```
  ./build/schedgen schedstimes.dat fcfs                                                                                                       
  ```
   ```
  ./build/schedgen schedstimes.dat sjf                                                                                                              
  ```
   ```
  ./build/schedgen schedstimes.dat q 4                                                                                                             
  ```
  Y una posible salida para la ejecución de este programa suponiendo que el algoritmo a ejecutar es SJF con el siguiente dataset es:
  ```
  0 3
  2 6 
  4 4
  6 5
  8 2
  ```
  ```
  1: runs 0-3 -> end = 3, (arr = 0), turn = 3, (burst = 3), wait = 0
  2: runs 3-4
  3: runs 4-8 -> end = 8, (arr = 4), turn = 4, (burst = 4), wait = 0
  5: runs 8-10 -> end = 10, (arr = 8), turn = 2, (burst = 2), wait = 0
  2: runs 10-15 -> end= 15, (arr = 2), turn = 13, (burst = 6), wait = 7
  4: runs 15-20 -> end= 20, (arr = 6), turn = 14, (burst = 5), wait = 9 
  
  Average turnaround time = 7.20
  Average normalized turnaround time = 1.59
  Average waiting time = 3.20
  
* El tercer programa denominado **schedstadistcs** consiste en un generador de datos estadísticos, correspondientes al análisis de procesos, como:
  * Tiempo promedio de respuesta de procesos por agrupacón de busrt time.
  * Tiempo promedio de respuesta normalizado de procesos por agrupación de burst time.
  * Tiempo promedio de espera de procesos por agrupación de burst time.
  Estos datos los genera en base a los algoritmos de agendamiento de procesos antes mencionado, específicamente emplea los algoritmos de RR1, RR4, FCFS y SJF apropiativo, por lo   que ejecuta los 4 algoritmos de agendamiento de manera concurrente, una posible salida por consola respecto al estado del programa sería la siguiente:
  ```
  procesando fcfs ...
  procesando jfc ... 
  procesando rr1 ...
  procesando rr4 ...
  procesando rr1 ...
  procesamiento finalizado (rr1)
  ```
  Para su ejecución es necesario escribir el siguiente comando:
  ```
  ./build/schedstats filename
  ```
  Que recibe únicamente un argumento:
  * **filename:** es una cadena de caracteres que le indica al programa que dataset de prueba usará para su simulación y posterior generación de 3 archivos cada uno correspondiente a la información estadística antes mencionada, cada uno de estos archivos sigue el siguiente formato:
  ```
  1 2 3 4 9 11
  0.1 0.8 0.9 0.15 0.2 
  0.8 0.7 0.10 0.8 0.3
  0.8 0.56 0.15 0.24 0.52
  0.17 0.12 0.25 0.36 0.8
  ```
  Donde:
  * La primera fila corresponde al burst time agrupado ascendente de todos los procesos analizados.
  * La segunda fila corresponde a los datos relacionados a alguno de los indicadores estadísticos correspondientes al algoritmo de FCFS vinculados a la agrupación de procesos de la primera fila.
  * La tercera fila corresponde a los datos relacionados a alguno de los indicadores estadísticos correspondientes al algoritmo de RR1 vinculados a la agrupación de procesos de la primera fila.
  * La cuarta fila corresponde a los datos relacionados a alguno de los indicadores estadísticos correspondientes al algoritmo de RR4 vinculados a la agrupación de procesos de la primera fila.
  * La quinta fila corresponde a los datos relacionados a alguno de los indicadores estadísticos correspondientes al algoritmo de SJF vinculados a la agrupación de procesos de la primera fila.
  
* El cuarto y último programa denominado **ploter.py** corresponde a un graficador de datos estadíticos, que a diferencia de los otros programas esta hecho en python y usa la librería de gráficación matplotlib, este programa no recibe ningún tipo de argumento, pero es necesario que los archivos que vaya a emplear para gráficar se encuentren en su mismo directorio y tengan los siguientes nombres:
  * **schedturns.dat** (Para realizar la gráfica de los tiempos de retorno vs burst time de procesos agrupados).
  * **schednturns.dat** (Para realizar la gráfica de los tiempos de retorno normalizados vs burst time de procesos agrupados).
  * **schedwaits.dat** (Para realizar la gráfica de los tiempos de espera vs burst time de procesos agrupados)
 Como se menciono este programa emplea la librería matplotlib y por lo tanto el interprete de Python 3.x.x. En caso de no tener dicha librería se debe instalar mediante los siguientes comandos en la terminal:
  ```
  python3 -m pip install -U pip
  python3 -m pip install -U matplotlib
  ```
  Una vez instalada la librería correspondiente y constar con Python3 el comando de ejecución de **ploter.py** es el siguiente:
  ```
   python3 src/ploter.py
  ```
  Las gráficas generadas por este programa tienen el mismo nombre que los archivos que alimentan la data de gráficación, salvo que llevan la extención **.jpeg**. Algunas de las imágenes generadas por este programa lucen de la siguiente manera:
  * **Tiempos de retorno vs Burst time de procesos agrupados**
  ![Tiempos de retorno vs Burst time de procesos agrupados](https://github.com/DevilRyu/Scheduler-Simulator/blob/master/resourcesGH/schedturns.jpeg)
  * **Tiempos de retorno normalizados vs Burst time de procesos agrupados**
  ![Tiempos de retorno normalizados vs Burst time de procesos agrupados](https://github.com/DevilRyu/Scheduler-Simulator/blob/master/resourcesGH/schednturns.jpeg)
  * **Tiempos de espera vs Burst time de procesos agrupados**
  ![Tiempos de espera vs Burst time de procesos agrupados](https://github.com/DevilRyu/Scheduler-Simulator/blob/master/resourcesGH/schedwaits.jpeg)
  
## Consideraciones:
- No eliminar el directorio **result** si es que se desea ejecutar los programas debido a que aquí es donde se alojan los resultados de los archivos.
- Tratar de llevar a cabo la ejecución de los programas secuencialmente (schedgen, schedsim, schedstats, ploter.py) debido a que si analizamos el flujo de ejecución del proyecto la salida de un programa puede servir como entrada del siguiente.
- El programa **schedsim**  esta hecho con hilos siguiendo un patrón productor-consumidor por lo que se hace uso de mutex y variables de condición, es por esto que se requiere un control de los items producidos por el productor para que el consumidor no se vea en la situación de consumir items no creados, por lo tanto si se revisa el código fuente de motor.c se notará que se duerme al proceso por 1 segundo para poder evitar esta condición en especial al comienzo del programa.
- Si si quisiera acelerar la ejecución del programa, podriá dormirse al proceso por menos tiempo, sin embargo considere que en ese caso puede llegarse a una condición de **deadlock** debido que a pesar de que esta es una simulación de un agendador de procesos, no deja de ser otro programa que dene ser administrado por el sistema operativo y su propio agendador de proceso. Por lo que para el obtener el mejor rendimiento del programa se recomiendo dormir los procesos por **1 segundo**
- El programa **schedstats** usa programación concurrente debido a que usa varios procesos para ejecutar los diferente algoritmos de agendamientos, es por ello que se ve ligado también a la consideración anterior.
- En caso de que desee acelerar la ejecución durmiendo por menos tiempo los procesos y se llega a un deadlock re-ejecutar el programa, de este modo el agendador del Sistema Operativo puede que lo encole de forma diferente pudiendo llegar a una ejecución óptima.

## Realizador por:                                                                                                    
* Diego Muñoz     
