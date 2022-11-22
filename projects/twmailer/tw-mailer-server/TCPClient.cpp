#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>

#include "TCPRequest.cpp"


#define BUF 1024

class TCPClient {
    public:
        TCPClient(char * server, char * port);
        ~TCPClient();
        void run();
    private:
        bool setup;
        int cli_socket;
        char buffer[BUF];
        struct sockaddr_in address;
        int size;
        TCPRequest * request;
        bool clientMenu(int socket);
        bool checkPort(char * portnumber);


};


TCPClient::TCPClient(char * server, char * port)
{
    setup = false;
    if(!checkPort(port))
    {
        printf("port must be a number\n") ;
        return;
    }
   

    if ((cli_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket failure\n\n") ;
        return;
    }

    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (atoi(port));
    inet_aton(server, &address.sin_addr);
    if(!(connect(cli_socket, (struct sockaddr *) &address, sizeof(address)) == 0))
    {
        printf("serverconnection could not be established\n") ;
        return;
    }
    printf ("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));
    request = new TCPRequest();

    setup = true;
}

TCPClient::~TCPClient()
{

}

void TCPClient::run()
{
    if(!setup)
    {
        printf("Client could no be started\n") ;
        return;
    }
    while(clientMenu(cli_socket))
    {
        system("clear");
        printf("-----------Server-----------\n");
        size = recv(cli_socket, buffer, BUF-1, 0);
        if (size > 0)
        {
            buffer[size] = '\0';
            printf("%s\n", buffer); 
        }
        printf("----------------------------\n");
        
    }
    

    close (cli_socket);
}



//------------------privat----------------------//
bool TCPClient::clientMenu(int cli_socket){
    char input;
    
    cout << "Please select one of the following options:" << endl;
    cout << "(L)ogin (S)end, L(I)st, (R)ead, (D)elete, (Q)uit \n\n" << endl;

    cout << ":>";
    cin >> input;

    string message;

    switch(input)
    {
        case 'S':
        case 's':
            message = request->sendMail();
            break;

        case 'L':
        case 'l':
            message = request->loginMail();
            break;

        case 'R':
        case 'r':
            message = request->readMail();
            break;

        case 'D':
        case 'd':
            message = request->deleteMail();
            break;

        case 'Q':
        case 'q':
            message = request->quitMail();
            return false;
            
        
        case 'I':
        case 'i':
            message = request->listMail();
            break;
        
        default:
            printf("unknown option\n") ;
            return true;
        
    }

    //printf("%s\n", message.c_str());
    send(cli_socket, message.c_str(), strlen(message.c_str()), 0);
    
    return true;
}

bool TCPClient::checkPort(char * portnumber)
{
    int i = 0;
    while(portnumber[i] != '\0')
    {
        int test = portnumber[i];
        if(test >= '0' && test <= '9')
        {
            i++;
        }
        else
        {
            return false;
        }
    }
    return true;
}