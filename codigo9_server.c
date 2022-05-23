#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

// Puerto a utilizar 
#define MYPORT 8080
// Conexiones admitidas en cola 
#define BACKLOG 10
// Máximo número de bytes que se pueden obtener 
#define MAXDATASIZE 300


int main(int argc, char *argv[]){
    
  /*Variables para creación de sockets*/
  int sockfd, new_fd, numbytes; // File descriptors
  struct sockaddr_in my_addr;   // Socket conexiones en el server 
  struct sockaddr_in their_addr;   // Socket para conexiones entrantes 
  int sin_size;   // Longitud de la estrctura 
  char bufferIn[MAXDATASIZE];   //bufferOut para datos

  /*Variables para ejecución de comandos*/
  FILE *read_fp; // Archivo que recibe el flujo de salida
  char bufferOut[BUFSIZ+1]; // Almacena contenido de read_fp
  int chars_read; // Tamaño del bufferOut

  //Creación del file descriptor
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("SERVER ERROR: Falló la creación de sockfd");
    exit(1);
  }
  else{ 
    printf("SERVER: sockfd correcto \n");
  }

  // Utilizamos la familia de sockets AF_INET
  // para realizar conexiones usando internet
  my_addr.sin_family = AF_INET;

  // Definimos el puerto a usar 
  // htons ordena los bytes para que sean reconocidos por la red 
  my_addr.sin_port = htons(MYPORT);

  // Declaramos la IP por la cual recibiremos datos 
  // INADDR_ANY habilita todas las interfaces de red del server 
  my_addr.sin_addr.s_addr = INADDR_ANY;
  printf("SERVER: IP= %s Puerto= %d \n", inet_ntoa(my_addr.sin_addr), MYPORT);

  // Inicilizamos con 0 la estructura 
  memset(&(my_addr.sin_zero), '\0', 8);

  // Asociamos el socket al file descriptor
  if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
    perror("SERVER ERROR: Falló la operación bind");
    exit(1);
  }
  else{
    printf("SERVER: bind correcto \n");
  }

  // Comenzamos a escuchar peticiones
  if(listen(sockfd, BACKLOG) == -1){
    perror("SERVER ERROR: Falló la operación listen");
    exit(1);
  }
  else{
    printf("SERVER: escuchando peticiones \n");
  }

  // Tamaño de la estructura
  sin_size = sizeof(struct sockaddr_in);

  while(1)
  {
    // Aceptamos las conexiones entrantes
    if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
      perror("SERVER ERROR: Falló la operación accept");
      exit(1);
    }

    printf("SERVER: accept correcto \n");
    printf("SERVER: new_fd correcto \n");
    printf("SERVER: Conexion entrante de: %s\n", inet_ntoa(their_addr.sin_addr));

    do{
      // Recibimos la información que envían
      if((numbytes = recv(new_fd, bufferIn, MAXDATASIZE-1, 0)) == -1){
        perror("SERVER ERROR: Falló la operación recv");
        exit(1);
      }

      memset(bufferOut,'\0',sizeof(bufferOut)); //bufferOut para salida de comandos
      bufferIn[numbytes] = '\0'; //bufferOut para datos enviados por cliente      
      printf("%s > %s", inet_ntoa(their_addr.sin_addr) ,bufferIn);

      read_fp=popen(bufferIn,"r");
      chars_read=fread(bufferOut, sizeof(char),BUFSIZ,read_fp);
      if(chars_read>0)
        send(new_fd, bufferOut, strlen(bufferOut), 0);
      
      pclose(read_fp);

    }while(!(strcmp(bufferIn,"exit\n")==0));
    
    close(new_fd);
    printf("SERVER: Conexion cerrada exitosamente \n");
  }
  close(sockfd);
  return 0;
}