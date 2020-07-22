# Scheduler-Simulator
Este proyecto consiste en una serie de programas que tienen como fin llevar a cabo una simulación de como es el agendamiento de procesos en un computador, los algoritmos de agendamiento usados fueron: FCFS, SJF, RR. Y los programas implementados son:

* ./schedgen (scheduler generator): programa cuyo fin es generar un archivo con los datos necesarios (time arrive, time burst) para llevar a cabo la simulación.
* ./schedsim (scheduler simulator): es el motor de simulación, este programa realiza todo el proceso de encolamiento e interrución de procesos y muestra como va la ejecución de manera detallada, utiliza programación en paralelo mediante el uso de hilos.
* ./schedstadistcs (scheduler stadistics): usa el motor de antes mencionado pero a diferencia del anterior este programa ejecuta 4 simulaciones al mismo tiempo con los algoritmos de rr1, rr4, sfj, fcfs; por lo que usa programación concurrente mediante procesos y como resultado genera 3 archivos con datos estadísticos para su posterior procesamiento.
* ./plotter: este programa toma los archivos del programa anterior para realizar su procesamiento y graficación mediante la librería matplotlib.

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
  * **filename:** es una cadena de cácteres que le indica al programa como se llamará el archivo que contenga los datos de prueba.
  Un ejemplo de su uso sería:
  ```
  ./build/schedgen 100 0.5 0.2 schedstimes.dat                                                                                                               
  ```
  
* El segundo programa denominado **schedsim** consiste en un motor de simulación que tiene 3 funcionalidades dependiendo de los parametros de entrada que recibe por consola este simulador, consta de 3 algortimos que son FCFS, RR y SJF apropiativos, el comando para ejecutar sus diferentes funcionalidades es:
  ```
  ./build/schedsim filename algoritmo [q]                                                                                                              
  ```
  Este programa consta principalmente de 2 paramentros obligatorios en cualquier ejecución que son:
  * **filename:** es una cadena de cácteres que le indica al programa que dataset de prueba usará para su simulación.
  * **algoritmo:** es una cadena de cácteres que le indica al programa que algoritmo de agendamiento de procesos usar.
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
  ```
<!--Si se requiere ayuda y saldrá un mensaje como este:
```
chat_server distributes encrypted chat messages between connected clients.

Usage:
  chat_server [-d] <port>
  chat_server -h

Options:
  -h             Help, show this screen.
  -d             Daemon mode.
```
Si se desea que se levante el servidor se usa:                                                                                         
```
./chat_server 8080
```
Si se desea que el servidor está ejecutandose en segundo plano se introduce:                                                         
```
./chat_server -d 8080
```
Para conocer si el servidor está corriendo en el segundo plano, se recomienda el siguiente comando:                   
```
lsof -i                                                                              
```
Para acabar con el proceso del servidor(siempre y cuando esté en segundo plano) se introduce el comando:
```
sudo kill <pid_del_proceso>
```
En cuanto al **cliente**, se requiere dos o más clientes se introduce los siguientes comandos:                                                                 
```
./chat_client –h                                                                                                                 
```
Si se requiere ayuda, se mostrará el siguiente mensaje:
```
./chat_client –h
chat_client connects to a remote chat_server service, allows the user to send messages to the chat group and displays chat messages from other clients.

Usage:
  chat_client –u <user> -p <password> <ip> <port>
  chat_client -h

Options:
  -h             Help, show this screen.
  -u             Specify the username.
  -p             Specify the password.
```
Para que un usuario se conecte se usa:                                                                                                 
```
./chat_client -u <nombre_usuario> -p <contraseña> <ip> <puerto>
```
Cada vez que un cliente se conecta se anuncia esa notificación en los usuarios ya conectados -->                                            

## Realizador por:                                                                                                    
* Diego Muñoz     
