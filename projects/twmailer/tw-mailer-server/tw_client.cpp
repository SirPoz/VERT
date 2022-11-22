/* myclient.c */

#include "TCPClient.cpp"



int main (int argc, char **argv) {
  
  if (argc < 3 ){
     printf("Usage: %s ServerAdresse Port\n", argv[0]);
     return 0;
  }

  TCPClient client = TCPClient(argv[1],argv[2]);
  client.run();
  
  return 0;
}
