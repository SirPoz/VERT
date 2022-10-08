/* myserver.c */
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

int main (void) {
  int srv_socket;
  socklen_t addrlen;
  char buffer[BUF];
  int size;
  struct sockaddr_in address, cliaddress;

  srv_socket = socket(AF_INET, SOCK_STREAM, 0);

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(srv_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
     perror("bind error");
     return EXIT_FAILURE;
  }
  
  if (listen(srv_socket, 5) != 0) {
     perror("listen failed");
     return EXIT_FAILURE;
  }
  
  addrlen = sizeof(struct sockaddr_in);

  while (1) {
     printf("Waiting for connections...\n");
     
     int comm_socket = accept(srv_socket, (struct sockaddr *) &cliaddress, &addrlen );
     if (comm_socket > 0)
     {
        printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
        strcpy(buffer,"Welcome to myserver, Please enter your command:\n");
        send(comm_socket, buffer, strlen(buffer),0);
     }

     do {
        size = recv(comm_socket, buffer, BUF-1, 0);
        if (size > 0)
        {
           buffer[size] = '\0';
           printf("Message received: %s\n", buffer);
        }
        else if (size == 0)
        {
           printf("Client closed remote socket\n");
           break;
        }
        else
        {
           perror("recv error");
           return EXIT_FAILURE;
        }
     } while (strncmp (buffer, "quit", 4)  != 0);

     close(comm_socket);
  }

  close(srv_socket);
  return EXIT_SUCCESS;
}
