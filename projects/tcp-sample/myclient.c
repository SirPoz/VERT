/* myclient.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF 1024
#define PORT 6543

int main (int argc, char **argv) {
  int cli_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;

  if (argc < 2 ){
     printf("Usage: %s ServerAdresse\n", argv[0]);
     exit(EXIT_FAILURE);
  }

  if ((cli_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
     perror("Socket error");
     return EXIT_FAILURE;
  }
  
  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons (PORT);
  inet_aton(argv[1], &address.sin_addr);

  if (connect(cli_socket, (struct sockaddr *) &address, sizeof(address)) == 0)
  {
     printf ("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));
     size = recv(cli_socket, buffer, BUF-1, 0);
     if (size > 0)
     {
        buffer[size] = '\0';
        printf("%s", buffer);
     }
  }
  else
  {
     perror("Connect error - no server available");
     return EXIT_FAILURE;
  }

  do {
     printf("Send message: ");
     fgets(buffer, BUF, stdin);
     send(cli_socket, buffer, strlen(buffer), 0);
  } 
  while (strcmp(buffer, "quit\n") != 0);

  close (cli_socket);

  return EXIT_SUCCESS;
}
