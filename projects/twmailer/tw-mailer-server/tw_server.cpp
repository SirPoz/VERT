/* myserver.c */
#include "TCPServer.cpp"


using namespace std;

int main(int argc, char **argv)
{
  

   if (argc < 2)
   {
      printf("Usage: ./twmailer-server <port (int)> <mail-spool-directoryname (string)>");
      exit(EXIT_FAILURE);
   }

   TCPServer server = TCPServer(argv[1],argv[2]);
   server.start();

   return 0;
}
