#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <pthread.h>
#include <vector>

#include "TCPParse.cpp"
#include "TCPProcessor.cpp"
#include "ldap.cpp"

using namespace std;
class TCPServer{
    public:
        TCPServer(char * port, char * spool);
        ~TCPServer();
        void run(string spool, int srv_socket,  socklen_t addrlen, struct sockaddr_in cliaddress);
        void start();
    private:
        //setup
        bool setup;
        int port;
        string spoolpath;

        bool checkPort(char * portnumber);
        bool createSpool(char * spool);
        string convertToString(char * charArray);
        static bool login(TCPParse * parse);

        //network
        int srv_socket;
        socklen_t addrlen;
        struct sockaddr_in address, cliaddress;
        pthread_t threads[20];
};

TCPServer::TCPServer(char * portNumber, char * spoolname)
{
    setup = false;
    if(checkPort(portNumber))
    {
        sscanf(portNumber,"%d",&port);
        printf("port set\n") ;
    }
    else
    {
        printf("port must be a number\n") ;
        return;
    }
    if(!createSpool(spoolname))
    {
        return;
    }
    printf("spool set\n") ;

    srv_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(srv_socket, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
        printf("bind unsuccessful\n") ;
        return;
    }
    printf("bind successful\n") ;
    

    if (listen(srv_socket, 5) != 0)
    {
        printf("listen unsuccessful\n") ;
        return;
    }
    printf("listen successful\n") ;

    addrlen = sizeof(struct sockaddr_in);

    printf("setup complete\n") ;
    setup = true;
}

TCPServer::~TCPServer()
{

}

void TCPServer::start()
{
    int forks = 5;
    for(int i = 0; i < 5; i++)
    {
        pid_t pid;
        pid = fork();
        switch(pid)
        {
            case -1:
                printf("failed to fork");
                break;
            case 0:
                run(this->spoolpath,this->srv_socket,this->addrlen,this->cliaddress);
                forks--;
                return;
                break;
            case 1:
                
                break;
        }
    }   
    while(forks > 0)
    {
        //do nothing
    }
    
}

void TCPServer::run(string spool, int srv_socket,  socklen_t addrlen, struct sockaddr_in cliaddress)
{
    if(!setup)
    {
        printf("server could not be started\n") ;
        return;
    }
    while (1)
    {
        printf("\nServer %d waiting for connection...\n", getppid());
        int comm_socket = accept(srv_socket, (struct sockaddr *)&cliaddress, &addrlen);
        if (comm_socket > 0)
        {
            printf("client connected from %s:%d\n",inet_ntoa(cliaddress.sin_addr),ntohs(cliaddress.sin_port));
            int size;
            int BUF = 1024;
            char buffer[BUF];
            string username;
            int logintry = 0;
            bool authenticated = false;
            
            while(comm_socket > 0)
            {
                
                if(comm_socket <= 0)
                {
                    break;
                }
               
                size = recv(comm_socket, buffer, BUF-1, 0);
                buffer[size] = '\0';

                TCPParse parser = TCPParse(buffer);
                
                if(strcmp(parser.method.c_str(), "QUIT") == 0)
                {
                    break;
                }
                else if(strcmp(parser.method.c_str(), "LOGIN") == 0)
                {
                    if(authenticated)
                    {
                        strcpy(buffer,"ERR\nAlready logged in");
                        send(comm_socket, buffer, strlen(buffer),0);
                    }
                    else
                    {
                        /*Ldaphandler ldap = Ldaphandler();
                        switch(ldap.login(parser.username,parser.password))
                        {
                            case LDAP_LOGIN_SUCCESS:
                                username = parser.username;
                                authenticated = true;
                                strcpy(buffer,"OK\n");
                                send(comm_socket, buffer, strlen(buffer),0);
                                break;
                            case LDAP_LOGIN_FAILED:
                                strcpy(buffer,"ERR\n");
                                send(comm_socket, buffer, strlen(buffer),0);
                                logintry++;
                                break;
                            case LDAP_LOGIN_ERROR:
                                strcpy(buffer,"ERR\nldapserver not reachable");
                                send(comm_socket, buffer, strlen(buffer),0);
                                break;
                        }*/
                        strcpy(buffer,"OK\n");
                        send(comm_socket, buffer, strlen(buffer),0);
                        authenticated = true;
                        username = parser.username;
                    }
                }
                else if(authenticated && strcmp(parser.method.c_str(), "ERROR"))
                {
                    TCPProcessor process = TCPProcessor(spool, username, comm_socket, &parser);
                    string response = process.getMessage();
                    
                    strcpy(buffer,response.c_str());
                    send(comm_socket, buffer, strlen(buffer),0);
                }
                else
                {
                    strcpy(buffer,"ERR\nlogin first");
                    send(comm_socket, buffer, strlen(buffer),0);
                }

                            
                
            }
           
        }

        close(comm_socket);
    }
}






//------------------privat-----------------------//

bool TCPServer::checkPort(char * portnumber)
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

bool TCPServer::createSpool(char * spoolname)
{
//create Directory if it doesnt already exist
    if (mkdir(spoolname, 0777) == -1)
    {
        printf("spoolfolder already exists\n");
        this->spoolpath = this->spoolpath + convertToString(spoolname);
    }
    else
    {
        printf("new spoolfolder was created\n") ;
        this->spoolpath = this->spoolpath + convertToString(spoolname);
    }

    //check if spoolfolder is accessible
    struct stat info;
    if( stat( this->spoolpath.c_str(), &info ) != 0 )
    {
        //information about spoolfolder coule not be accessed
        printf("spoolfolder could not be accessed\n") ;
        return false;
    }
    else if( info.st_mode & S_IFDIR )
    {
        //information was accessed and is a directory
        printf("spoolfolder could be accessed correctly\n") ;
        return true;
    }
    else
    {
        //information was accessed but is not a directory
        printf("spoolfolder could not be interpreted as folder\n") ;
        return false;
    }
}

string TCPServer::convertToString(char * charArray)
{
    string result = "";
    int i = 0;
    while(charArray[i] != '\0')
    {
        result = result + charArray[i];
        i++;
    }

    return result;
}

