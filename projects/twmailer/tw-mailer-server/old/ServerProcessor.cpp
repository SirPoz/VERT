#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <vector>
#include <regex>
#include <pthread.h>

#include "ldap.cpp"

#define BUF 1024

class ServerProcessor{
    public:
        ServerProcessor(int client, string spool, string userip, string threadid)
        {
            this->ldap = new Ldaphandler();
            this->clientsocket = client;
            this->spoolpath = spool;
            this->authenticated = false;
            this->useradress = userip;
            this->threadid = threadid;
        }

        void run()
        {
            
            while(clientsocket > 0)
            {

                if (clientsocket <= 0)
                {
                    return;
                }
                if(blacklist(false))
                {
                    sendResponse("ERR\nYou have been blocked. Please try again later");
                    return;
                } 




                size = recv(clientsocket, buffer, BUF-1, 0);
                
                buffer[size] = '\0';
                this->currentstream.str("");
                this->currentstream.clear();
                this->currentstream << buffer;

                getline(this->currentstream, message);
               
                //if authenticated the user will have access to all functions of the server
                switch(checkMethod(message.c_str()))
                    {
                        case SEND:
                            (this->authenticated) ? this->sendEmail() : sendResponse("ERR\nlogin first!");
                            break;
                        case LIST:
                            (this->authenticated) ? this->listEmail() : sendResponse("ERR\nlogin first!");
                            break;
                        case READ:
                            (this->authenticated) ? this->readEmail() : sendResponse("ERR\nlogin first!");
                            break;
                        case DEL:
                            (this->authenticated) ? this->delEmail() : sendResponse("ERR\nlogin first!");
                            break;
                        case LOGIN:
                            this->login();
                            break;
                        case QUIT:
                            return;
                            
                        default:
                            this->sendResponse("ERR\nUnknown option");
                            break;
                    }
                
            }
        }


    private:
        string useradress;
        bool authenticated;
        string username;
        int clientsocket;
        string spoolpath;
        Logger * log;
        int size; 
        char buffer[BUF];
        string message;
        char Cmd[5];
        Ldaphandler * ldap;
        stringstream currentstream;
        string threadid;

        enum methods {SEND, LIST, DEL, READ, QUIT, LOGIN, ERROR};


        methods checkMethod(const char * message)
        {

            if(strcmp(message, "SEND") == 0){
                return SEND;
            }
            if(strcmp(message, "LIST") == 0){
                return LIST;
            }
            if(strcmp(message, "READ") == 0){
                return READ;
            }
            if(strcmp(message, "DEL") == 0){
                return DEL;
            }
            if(strcmp(message, "QUIT") == 0){
                return QUIT;
            } 
            if(strcmp(message, "LOGIN") == 0){
                return LOGIN;
            } 

            return ERROR;
        }


        void login()
        {
            string function;
            string temp_user;
            string temp_pass;
            int ldapresult;

            getline(this->currentstream,function);
            getline(this->currentstream,temp_user);
            getline(this->currentstream,temp_pass);

          

            ldapresult = this->ldap->login(temp_user,temp_pass);

            switch(ldapresult)
            {
                case LDAP_LOGIN_SUCCESS:
                    this->authenticated = true;
                    break;
                case LDAP_LOGIN_FAILED:
                    blacklist(true);
                    break;
                case LDAP_LOGIN_ERROR:
                    break;
                default:
                    break;
            }

            if(this->authenticated)
            {
                this->username = temp_user;
                sendResponse("OK\n");
            }
            else
            {
                sendResponse("ERR\n");
            }
        }

        void listEmail()
        {
            //filehandler->list(this->username);
        }

        void readEmail()
        {
            
        }

        void delEmail()
        {

        }

        void sendEmail()
        {

        }

        void sendResponse(string response)
        {    
            string responsebody = response + "\n" + this->threadid;
            strcpy(this->buffer, responsebody.c_str());
            send(this->clientsocket, buffer, strlen(buffer),0);
        }

        bool blacklist(bool failedattempt)
        {
         /*   if(failedattempt)
            {
                filehandler->addBlacklist(this->useradress);
                this->log->LogWarning("failed login attempt by " + this->useradress);
            }

            int attempts;
            attempts = filehandler->getBlacklist(this->useradress);

            return (attemps >= 3) ? true : false;*/

            return failedattempt;

        }
};