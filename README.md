# Scheduler-Simulator
Este proyecto consiste en una serie de programas que tienen como fin llevar a cabo una simulación de como es el agendamiento de procesos en un computador, los algoritmos de agendamiento usados fueron: FCFS, SJF, RR. Y los programas implementados son:

* ./schedgen: programa cuyo fin es generar un archivo con los datos necesarios (time arrive, time burst) para llevar a cabo la simulación.
* ./schedsim: es el motor de simulación, este programa realiza todo el proceso de encolamiento e interrución de procesos y muestra como va la ejecución de manera detallada, utiliza programación en paralelo mediante el uso de hilos.
* ./schedstadistcs: usa el motor de antes mencionado pero a diferencia del anterior este programa ejecuta 4 simulaciones al mismo tiempo con los algoritmos de rr1, rr4, sfj, fcfs; por lo que usa programación concurrente mediante procesos y como resultado genera 3 archivos con datos estadísticos para su posterior procesamiento.
* ./plotter: este programa toma los archivos del programa anterior para realizar su procesamiento y graficación mediante la librería matplotlib.

En la siguiente sección se explicará más a detalle cada uno de los programas mencionados.


## Modo de uso y compilación
Para poder compilar los programas realizados en C es necesario introducir en la terminal el siguiente comando:                                                       
```
make
```
Se debe tener una terminal para el servidor y dos o más para cliente;                                                               
En el **servidor** se debe introducir:
<!--```
./chat_server –h                                                                                                                 
```
Si se requiere ayuda y saldrá un mensaje como este:
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
