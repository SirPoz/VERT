#pragma once
#include <string>
#include <sstream>
#include <iostream>

using namespace std;


class TCPParse{
    public:
        TCPParse(char * buffer);
        
        string method;
        string username;
        string password;
        string receiver;
        string subject;
        string body;
        int number;
        

    private:
        stringstream protocol;
        void parseSend();
        void parseRead();
        void parseDelete();
        void parseLogin();
        void checkmethod();
        bool is_number(const string& s);
};

TCPParse::TCPParse(char * buffer)
{
    protocol << buffer;
    getline(protocol,method);
    checkmethod();
}


//------------------privat---------------------//
void TCPParse::checkmethod()
{
    if(strcmp(method.c_str(), "SEND") == 0)
    {
        parseSend();
        return;
    }
    if(strcmp(method.c_str(), "LIST") == 0)
    {
        //do nothing
        return;
    }
    if(strcmp(method.c_str(), "READ") == 0)
    {
        parseRead();
        return;
    }
    if(strcmp(method.c_str(), "DEL") == 0)
    {
        parseDelete();
        return;
    }
    if(strcmp(method.c_str(), "QUIT") == 0)
    {
        //do nothing;
        return;
    } 
    if(strcmp(method.c_str(), "LOGIN") == 0)
    {
        parseLogin();
        return;
    } 

    method = "ERROR";
}

void TCPParse::parseSend()
{
    getline(protocol,receiver);
    getline(protocol,subject);

    string line;
    while(1)
    {
        getline(protocol,line);
        if(line == ".")
        {
            break;
        }
        else
        {
            body = body + "\n" + line;
        }
    }
    
}
void TCPParse::parseDelete()
{
    string messagenumber;
    getline(protocol,messagenumber);
    method = (is_number(messagenumber)) ? "DEL" : "ERROR";
    number = stoi(messagenumber);
}
void TCPParse::parseRead()
{
    string messagenumber;
    getline(protocol,messagenumber);
    method = (is_number(messagenumber)) ? "READ" : "ERROR";
    number = stoi(messagenumber);

}
void TCPParse::parseLogin()
{
    getline(protocol,username);
    getline(protocol,password);
}

bool TCPParse::is_number(const string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}