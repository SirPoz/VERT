#include "TCPParse.cpp"
#include "filehandler.cpp"
#include "model/Email.cpp"

class TCPProcessor{
    public:
        TCPProcessor(string spool, string username, int client, TCPParse * parse);
        string getMessage();
    private:
        filehandler * spoolhandler;  
        string message;  
        
};

TCPProcessor::TCPProcessor(string spool, string username, int client, TCPParse * parse)
{
    spoolhandler = new filehandler();

    if(strcmp(parse->method.c_str(), "SEND") == 0)
    {
        email mail = email(username, parse->receiver, parse->subject, parse->body);
        this->message = spoolhandler->writemailfile(&mail,spool);

    }
    if(strcmp(parse->method.c_str(), "LIST") == 0)
    {
         this->message = spoolhandler->create_mail_list(username, spool);
    }
    if(strcmp(parse->method.c_str(), "READ") == 0)
    {
         this->message = spoolhandler->readfile(username,parse->number,spool);
    }
    if(strcmp(parse->method.c_str(), "DEL") == 0)
    {
         this->message = spoolhandler->deletefile(username, parse->number,spool);
    }
    
    if(this->message.length() <= 0)
    {

        this->message = "ERR\n";
    }
    
}

string TCPProcessor::getMessage()
{
    return this->message;
}