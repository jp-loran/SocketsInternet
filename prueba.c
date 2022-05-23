#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(){
  FILE *read_fp;
  char buffer[BUFSIZ+1];
  int chars_read;

  memset(buffer,'\0',sizeof(buffer));
  read_fp=popen("ls -l ./","r");

  chars_read=fread(buffer, sizeof(char),BUFSIZ,read_fp);
  if(chars_read>0){
    printf("Data recieved is: \n %s \n", buffer);
  }
  pclose(read_fp);
  exit(EXIT_SUCCESS);
}
