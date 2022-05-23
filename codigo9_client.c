#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080 // Puerto del servidor
#define MAXDATASIZE 6000 // Número máximo de bytes a recibir

int main(int argc, char *argv[]){

  int sockfd, numbytes; //File descriptor 
  char buf[MAXDATASIZE]; //Buffer para recibir y enviar datos
  struct hostent *he; // Información del host
  struct sockaddr_in their_addr; //Informacion direcciones de conexión

  // Si no hay argumentos al ejecutar
  if(argc != 2){
    fprintf(stderr, "CLIENT ERROR: Dirección de servidor no especificada \n");
    exit(1);
  }

  // Obtenemos información del host
  if((he=gethostbyname(argv[1])) == NULL){
    perror("gethostbyname()");
    exit(1);
  }
  else
    printf("CLIENTE: El host remoto es -> %s\n", argv[1]);

  // Creación del file descriptor
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("socket()");
    exit(1);
  }
  else 
    printf("CLIENTE: File descriptor correcto \n");

  // Información del socket
  their_addr.sin_family = AF_INET; //socket internet
  their_addr.sin_port = htons(PORT); //puerto
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);// direccion
  printf("SERVER: IP= %s Puerto= %d \n", argv[1], PORT);


  // Inicializamos en cero el resto de la estructura
  memset(&(their_addr.sin_zero), '\0', 8);

  // Hacemos la conexión al server
  if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
    perror("connect()");
    exit(1);
  }
  else
    printf("CLIENTE: Conexión con servidor establecida\n");

  //Inicia la interaccion con el cliente
  while(printf("> "), fgets(buf, 100, stdin), !feof(stdin)){
    //Se captura y envía el server la información
    if(send(sockfd, buf, strlen(buf), 0) == -1)
      perror("Error al enviar");
    else{
      //Si se envía exit terminamos al ejecución
      if(strcmp(buf,"exit\n")==0)
        break;
      else
        memset( buf, '\0', sizeof( buf )); //limpiamos buffer
        numbytes=recv(sockfd, buf, MAXDATASIZE,0); //Recibimos respuesta del server
        buf[numbytes] = '\0'; //Se agrega el fin de archivo
        printf("%s> %s \n",argv[1], buf); 
    }
  }

  printf("CLIENTE: Cerrando conexión \n");
  close(sockfd);
  return 0;
}