/* myserver.c */
#include "SocketServer.cpp"
#include "Logger.cpp"





using namespace std;

int main(int argc, char **argv)
{
   Logger * logger = new Logger();

   if (argc < 2)
   {
      logger->LogError("Usage: ./twmailer-server <port (int)> <mail-spool-directoryname (string)>");
      exit(EXIT_FAILURE);
   }

   SocketServer * server = new SocketServer(argv[1],argv[2],logger);
   server->run();


   


   free(server);
   free(logger);
   return 0;
}
