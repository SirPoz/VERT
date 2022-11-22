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
#include <csignal>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "TCPParse.cpp"
#include "TCPProcessor.cpp"
#include "ldap.cpp"
#include "Logger.cpp"

using namespace std;


static vector<pid_t> forks;
static volatile sig_atomic_t running = 1;

void signalhandler(int sig);





class TCPServer{
    public:
        TCPServer(char * port, char * spool);
        ~TCPServer();
        void run(string spool, int srv_socket,  socklen_t addrlen, struct sockaddr_in cliaddress);
        void start();
        void stop();
    private:
        //setup
        bool setup;
        int port;
        string spoolpath;
        bool running;
        Logger * log = new Logger();

        bool checkPort(char * portnumber);
        bool createSpool(char * spool);
        string convertToString(char * charArray);
        static bool login(TCPParse * parse);
        bool checkBlock(string entry);
        
        //network
        int srv_socket;
        socklen_t addrlen;
        struct sockaddr_in address, cliaddress;
       
};

TCPServer::TCPServer(char * portNumber, char * spoolname)
{
    setup = false;
     
    

    if(checkPort(portNumber))
    {
        sscanf(portNumber,"%d",&port);
        log->LogSuccess("port set");
    }
    else
    {
        log->LogError("port must be a number") ;
        return;
    }
    if(!createSpool(spoolname))
    {
        return;
    }
    log->LogSuccess("spool set") ;

    srv_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(srv_socket, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
        log->LogError("bind unsuccessful") ;
        return;
    }
    log->LogSuccess("bind successful") ;
    

    if (listen(srv_socket, 5) != 0)
    {
        log->LogError("listen unsuccessful") ;
        return;
    }
    log->LogSuccess("listen successful") ;

    addrlen = sizeof(struct sockaddr_in);
    setup = true;

    log->LogSuccess("setup complete") ;
    
    fflush(stdout);
}

TCPServer::~TCPServer()
{
    
}

//starts server and forks it
void TCPServer::start()
{
    
    
    pid_t pid;
    for(int i = 0; i < 7; i++)
    {
        
        pid = fork();
        switch(pid)
        {
            case -1:
                
                return;

            case 0:
                
                run(this->spoolpath,this->srv_socket,this->addrlen,this->cliaddress);
                return;

            default:
                forks.push_back(pid);
                break;
        }
        
    }

    
    while(running)
    {
        
    
    }
    
    
}

void TCPServer::stop()
{
  for(auto & pid : forks) 
  {
    if(getpid() != (pid_t)pid)
    {
        kill((pid_t)pid, SIGINT);
    } 
  }
  close(srv_socket);

  return;
}


//----------------IPC---------------------------------------------------------------------------//

void TCPServer::run(string spool, int srv_socket,  socklen_t addrlen, struct sockaddr_in cliaddress)
{
    if(!setup)
    {
        printf("server could not be started\n") ;
        return;
    }
    while (running)
    {
        
        printf("\nServer %d waiting for connection...",getpid());
        int comm_socket = accept(srv_socket, (struct sockaddr *)&cliaddress, &addrlen);
               
            if (comm_socket > 0)
            {
                printf("client connected from %s:%d \n",inet_ntoa(cliaddress.sin_addr),ntohs(cliaddress.sin_port));
                int size;
                int BUF = 1024;
                char buffer[BUF];
                string username;
                int logintry = 0;
                bool authenticated = false;
                filehandler block = filehandler();

                if(!checkBlock(block.readblacklistentry(inet_ntoa(cliaddress.sin_addr))))
                {
                    size = recv(comm_socket, buffer, BUF-1, 0);
                    strcpy(buffer,"ERR\nstill blocked");
                    send(comm_socket, buffer, strlen(buffer),0);
                    close(comm_socket);
                    comm_socket = -1;
                }
                while(comm_socket > 0)
                {
                    if(logintry >= 3)
                    {
                        strcpy(buffer,"ERR\ntoo many failed login attempts");
                        send(comm_socket, buffer, strlen(buffer),0);
                        block.writeblacklistentry(inet_ntoa(cliaddress.sin_addr));
                        close(comm_socket);
                        comm_socket = -1;
                    }
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
                            Ldaphandler ldap = Ldaphandler();
                            switch(ldap.login(parser.username,parser.password))
                            {
                                case LDAP_LOGIN_SUCCESS:
                                    username = parser.username;
                                    authenticated = true;
                                    strcpy(buffer,"OK\n");
                                    send(comm_socket, buffer, strlen(buffer),0);
                                    break;
                                case LDAP_LOGIN_FAILED:
                                    strcpy(buffer,"ERR\ninvalid credentials");
                                    send(comm_socket, buffer, strlen(buffer),0);
                                    logintry++;
                                    break;
                                case LDAP_LOGIN_ERROR:
                                    strcpy(buffer,"ERR\nldapserver not reachable");
                                    send(comm_socket, buffer, strlen(buffer),0);
                                    break;
                            }
                            
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
                close(comm_socket);  
            }
                  
    }
    stop();
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
        log->LogInfo("spoolfolder already exists");
        this->spoolpath = this->spoolpath + convertToString(spoolname);
    }
    else
    {
        log->LogWarning("new spoolfolder was created");
        this->spoolpath = this->spoolpath + convertToString(spoolname);
    }

    //check if spoolfolder is accessible
    struct stat info;
    if( stat( this->spoolpath.c_str(), &info ) != 0 )
    {
        //information about spoolfolder coule not be accessed
        log->LogError("spoolfolder could not be accessed") ;
        return false;
    }
    else if( info.st_mode & S_IFDIR )
    {
        //information was accessed and is a directory
        log->LogSuccess("spoolfolder could be accessed correctly\n") ;
        return true;
    }
    else
    {
        //information was accessed but is not a directory
        log->LogError("spoolfolder could not be interpreted as folder") ;
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

bool TCPServer::checkBlock(string entry)
{
    if(entry == "ERR\n")
    {
        return true;
    }
    else
    {
        time_t current;
        time(&current);
        
        string time = entry.substr(entry.find('_'),entry.length());

       
        struct tm tm;
        strptime(time.c_str(), "_%Y-%m-%d %H:%M:%S", &tm);
        time_t t = mktime(&tm);
        printf(to_string(difftime(current, t)).c_str());
        printf("\n");
        return (difftime(current, t)>300);
              
    }
}


void signalhandler(int sig)
{
    running = 0;
    exit(sig);
}
