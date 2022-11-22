/* myserver.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "./handler/mailhandler.h"
//#include <windows>

#define BUF 1024
//#define PORT 6543

using namespace std;

int main(int argc, char **argv)
{
   int srv_socket;
   socklen_t addrlen;
   char buffer[BUF];
   int size;
   long port;
   char *p;
   string spoolpath = "./";
   struct sockaddr_in address, cliaddress;

   if (argc < 2)
   {
      cout << "Usage: ./twmailer-server <port> <mail-spool-directoryname>" << endl;
      exit(EXIT_FAILURE);
   }

   //create Directory if it doesnt already exist
   if (mkdir(argv[2], 0777) == -1)
   {

      spoolpath = spoolpath + argv[2];
   }
   else
   {
      cout << "Directory created" << endl;
      spoolpath = spoolpath + argv[2];
   }

   port = strtol(argv[1], &p, 10);

   cout << port << endl;

   cout << spoolpath << endl;

   srv_socket = socket(AF_INET, SOCK_STREAM, 0);

   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(port);

   if (bind(srv_socket, (struct sockaddr *)&address, sizeof(address)) != 0)
   {
      perror("bind error");
      return EXIT_FAILURE;
   }

   if (listen(srv_socket, 5) != 0)
   {
      perror("listen failed");
      return EXIT_FAILURE;
   }

   addrlen = sizeof(struct sockaddr_in);

   while (1)
   {
      printf("Waiting for connections...\n");

      int comm_socket = accept(srv_socket, (struct sockaddr *)&cliaddress, &addrlen);
      if (comm_socket > 0)
      {
         printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
         handlemail(comm_socket, spoolpath);
      }

      close(comm_socket);
   }

   close(srv_socket);
   return EXIT_SUCCESS;
}
