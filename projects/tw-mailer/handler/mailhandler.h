#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <vector>
#include <regex>
#include "filehandler.h"

#define BUF 1024


using namespace std;

enum commands { Send, List, Read, Del , Quit ,Error};

//Reads the command and returns the corresponding ID
commands CheckCommand(char *message){

    if(strcmp(message, "SEND") == 0){
        return Send;
    }
    if(strcmp(message, "LIST") == 0){
        return List;
    }
    if(strcmp(message, "READ") == 0){
        return Read;
    }
    if(strcmp(message, "DEL") == 0){
        return Del;
    }
    if(strcmp(message, "QUIT") == 0){
        return Quit;
    } 

    return Error;
}


void sendresponse(int clientsocket, string message){

         char buffer[BUF];   
         strcpy(buffer,message.c_str());
         send(clientsocket, buffer, strlen(buffer),0);

}




void sendmail(int clientsocket, stringstream &message, string spoolpath){

    string sender;
    string receiver;
    string subject;
    string body;
    string line;
    int size;
    char buffer[BUF];
   
    getline(message,sender,'\n');

    getline(message,receiver,'\n');
     
    getline(message,subject,'\n');


    if(sender.length() > 8 || receiver.length() > 8 || subject.length() > 80){

        sendresponse(clientsocket, "Err\n");
        cout << "error" << endl;
        return;
    }

   if (writefile(sender, receiver,subject, message,spoolpath)){

         sendresponse(clientsocket, "OK\n");
         
   }
   else{
         sendresponse(clientsocket, "Err\n");   
   }


}

void listmails(int clientsocket, stringstream &message, string spoolpath){


    string mailist;
    string user;
    char buffer[BUF];

    getline(message,user,'\n');
    
    mailist = create_mail_list(user, spoolpath);

    sendresponse(clientsocket,  mailist);

}


void readmail(int clientsocket,stringstream &message, string spoolpath){

     
     string userdirpath;
     string user;
     string filepath;
     string filename;
     string msgID;
     string messagemail;
     char buffer[BUF];


     getline(message,user,'\n');
     getline(message,msgID,'\n');

     filename = user + "_message" + msgID;

     userdirpath = spoolpath + "/" + user;

    filepath = search_for_file(userdirpath, filename);

    if(!strcmp(filepath.c_str(),"Error")){

         sendresponse(clientsocket, "Err\n");   
         return;
    }

    messagemail = readfile(filepath);


     
    sendresponse(clientsocket,  messagemail);   

}



void del_mail(int clientsocket,stringstream &message, string spoolpath){

    string userdirpath;
    string user;
    string filepath;
    string filename;
    string msgID;
    string messagemail;

    getline(message,user,'\n');
    getline(message,msgID,'\n');

    filename = user + "_message" + msgID;

    userdirpath = spoolpath + "/" + user;

    filepath = search_for_file(userdirpath, filename);

    if(!strcmp(filepath.c_str(),"Error")){

         sendresponse(clientsocket, "Err\n");   
         return;
    }


    //Checking if the message delition was successful or not
    if(remove(filepath.c_str()) == 0)
    {
        sendresponse(clientsocket, "OK\n");   
    }
    else
    {
        sendresponse(clientsocket, "Err\n"); 
    }

}


void handlemail(int clientsocket, string spoolpath){
    int size; 
    char buffer[BUF];
    string message;
    char Cmd[5];


    while(clientsocket > 0){

      if (clientsocket <= 0) {
            return;
        }  

        size = recv(clientsocket, buffer, BUF-1, 0);

        buffer[size] = '\0';
        printf("Message received: %s\n", buffer);

        stringstream stream(buffer); 

        getline(stream,message);

        strcpy(Cmd,message.c_str());

        switch(CheckCommand(Cmd))
        {
            case Send:

                cout << "SEND" << endl;
                sendmail(clientsocket,stream,spoolpath);

                break;

                case List :
           
                cout << "LIST" << endl;
                listmails(clientsocket,stream,spoolpath);

                break;

                case Read :
           
                 cout << "READ" << endl;
                 readmail(clientsocket,stream,spoolpath);

                break;

                case Del:
           
                 cout << "DEL" << endl;
                 del_mail(clientsocket,stream,spoolpath);

                break;

                case Quit:
           
                 cout << "QUIT" << endl;

                return;


                default: 

                cout << "UNKOWN COMMAND" << endl;

                strcpy(buffer,"UNKOWN COMMAND\n Please enter your command:\nSEND\nLIST\nREAD\nDEL\nQUIT\n");

                send(clientsocket, buffer, strlen(buffer),0);

                break;

            }
    }

}


