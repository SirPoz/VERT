#include "Logger.cpp"

#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <vector>
#include <regex>

#include "ldap.cpp"

#define BUF 1024

class ServerProcessor{
    public:
        ServerProcessor(int client, string spool, string userip, Logger * logger)
        {

            this->ldap = new Ldaphandler();
            this->clientsocket = client;
            this->spoolpath = spool;
            this->authenticated = false;
            this->log = logger;
            this->useradress = userip;
            
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
                    this->log->LogWarning("blocked IP tried to access");
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
                if(authenticated)
                {
                    switch(checkMethod(message.c_str()))
                    {
                        case SEND:
                            this->sendEmail();
                            break;
                        case LIST:
                            this->listEmail();
                            break;
                        case READ:
                            this->readEmail();
                            break;
                        case DEL:
                            this->delEmail();
                            break;
                        case QUIT:
                            (authenticated) ? this->log->Log(this->username + " quit its session") : this->log->Log("session was quit from " + this->useradress);
                            return;
                            break;
                        default:
                            this->sendResponse("ERR\nUnknown option");
                            break;
                    }
                }

                //if not authenticated the user will only be able to login or quit
                else
                {
                    switch(checkMethod(message.c_str()))
                    {
                        case LOGIN:
                            login();
                            break;
                        case QUIT:
                            (authenticated) ? this->log->Log(this->username + " quit its session") : this->log->Log("session was quit from " + this->useradress);
                            return;
                            break;
                        default:
                            this->log->LogWarning("unautherized access attempt by " + this->useradress);
                            sendResponse("ERR\nAuthenticate first to accesss all functions\n");
                            break;
                    }
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

        enum methods {SEND, LIST, DEL, READ, QUIT, LOGIN, ERROR};


        methods checkMethod(const char * message){

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

            log->LogInfo("User: " + temp_user);
            log->LogInfo("Pass: " + temp_pass);

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
                    this->log->LogError("LDAP Server could not be reached");
                    break;
                default:
                    this->log->LogWarning("LDAP functionality does not work");
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
            strcpy(this->buffer, response.c_str());
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