# Scheduler-Simulator
Aplicación cliente - servidor grupo de chat encriptado usando serialización CBOR y encriptación AES256.

## Modo de uso y compilación
Para poder compilar el programa es necesario ante todo introducir el comando                                                          
```
make
```
Se debe tener una terminal para el servidor y dos o más para cliente;                                                               
En el **servidor** se debe introducir:
```
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
Cada vez que un cliente se conecta se anuncia esa notificación en los usuarios ya conectados                                               

## Realizador por:                                                                                                    
* Diego Muñoz     
