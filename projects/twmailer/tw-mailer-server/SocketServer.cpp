#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <pthread.h>
#include "Logger.cpp"
#include "ServerProcessor.cpp"


#define BUF 1024

class SocketServer{
    public:
        

        SocketServer(char * portNumber, char * spoolname, pthread_mutex_t * logMutex){
            this->setup = false;
            this->log = new Logger(logMutex);
            this->mutex = logMutex;
            if(checkPort(portNumber))
            {
                this->log->LogSuccess("port could be intepreted as number");
                //convert char array into int type
                sscanf(portNumber,"%d",&this->port);
                if(createSpool(spoolname))
                {
                    this->log->LogSuccess("setup was successful");
                    this->setup = true;
                }
            }
        }

        void run(){
            if(!this->setup)
            {
                this->log->LogError("setup was not successful, server could not be started");
                return;
            }
            
            srv_socket = socket(AF_INET, SOCK_STREAM, 0);

            memset(&address, 0, sizeof(address));
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);

            if (bind(srv_socket, (struct sockaddr *)&address, sizeof(address)) != 0)
            {
                this->log->LogError("could not bind");
                return;
            }
            this->log->LogSuccess("binding successful");

            if (listen(srv_socket, 5) != 0)
            {
                this->log->LogError("listen failed");
                return;
            }
            this->log->LogSuccess("listening successful");

            addrlen = sizeof(struct sockaddr_in);

            while (1)
            {
                this->log->LogInfo("server waiting for connections\n");

                int comm_socket = accept(srv_socket, (struct sockaddr *)&cliaddress, &addrlen);
                if (comm_socket > 0)
                {
                    this->log->LogSuccess("client connected from " + convertToString(inet_ntoa(cliaddress.sin_addr)) + ":" + to_string(ntohs(cliaddress.sin_port))+"\n");
                    
                    threads.push_back(new ServerProcessor(comm_socket,this->spoolpath,convertToString(inet_ntoa(cliaddress.sin_addr)),this->mutex));
                              
                }

                close(comm_socket);
            }
            this->log->LogInfo("server shutting down");
            close(srv_socket);
            return;

        }

    private:
        bool setup;
        int srv_socket;
        socklen_t addrlen;
        char buffer[BUF];
        int size;
        int port;
        char *p;
        string spoolpath = "./";
        struct sockaddr_in address, cliaddress;
        vector<ServerProcessor *> threads;
        pthread_mutex_t * mutex;
        Logger * log;




        bool checkPort(char * portNumber)
        {
            int i = 0;
            while(portNumber[i] != '\0')
            {
                int test = portNumber[i];
                if(test >= '0' && test <= '9')
                {
                    i++;
                }
                else
                {
                    this->log->LogError("port could not be interpreted as number");
                    return false;
                }
            }
            return true;
        }

        string convertToString(char * charArray)
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

        bool createSpool(char * spoolname){

            //create Directory if it doesnt already exist
            if (mkdir(spoolname, 0777) == -1)
            {
                this->log->LogInfo("spoolfolder already exists");
                this->spoolpath= this->spoolpath + convertToString(spoolname);
            }
            else
            {
                this->log->LogWarning("new spoolfolder was created");
                this->spoolpath = this->spoolpath + convertToString(spoolname);
            }

            //check if spoolfolder is accessible
            struct stat info;
            if( stat( this->spoolpath.c_str(), &info ) != 0 )
            {
                //information about spoolfolder coule not be accessed
                this->log->LogError("spoolfolder could not be accessed");
                return false;
            }
            else if( info.st_mode & S_IFDIR )
            {
                //information was accessed and is a directory
                this->log->LogSuccess("spoolfolder could be accessed correctly");
                return true;
            }
            else
            {
                //information was accessed but is not a directory
                this->log->LogError("spoolfolder could not be interpreted as folder");
                return false;
            }

        }

       
};

