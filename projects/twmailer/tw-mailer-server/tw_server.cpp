/* myserver.c */
#include "SocketServer.cpp"
#include "Logger.cpp"
#include <pthread.h>


static pthread_mutex_t logmutex;

using namespace std;

int main(int argc, char **argv)
{
   pthread_mutex_init(&logmutex, NULL);
   Logger * logger = new Logger(&logmutex);

   if (argc < 2)
   {
      logger->LogError("Usage: ./twmailer-server <port (int)> <mail-spool-directoryname (string)>");
      exit(EXIT_FAILURE);
   }

   SocketServer * server = new SocketServer(argv[1],argv[2],&logmutex);
   server->run();


   free(server);
   free(logger);
   return 0;
}
