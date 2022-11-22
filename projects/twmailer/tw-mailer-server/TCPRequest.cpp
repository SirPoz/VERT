#include <string.h>
#include <iostream>
#include <string>

using namespace std;

class TCPRequest{
    public:
        string sendMail();
        string listMail();
        string readMail();
        string deleteMail();
        string quitMail();
        string loginMail();

    private:
        string convertToString(char * array);
        //string takePasswdFromUser();
        enum IN 
        { 
            IN_BACK = 8,
            IN_RET = 13
        };
    
};

string TCPRequest::sendMail(){
    string message;
    string line;
    string text;
    string subject;
   
    char receiver[9];
    

    cout << "Please enter receiver: ";
    cin >> receiver;

    cout << "Please enter subject: ";
    getline(cin >> ws, subject);

    cout << "Please enter message (close with a ;):" << endl;
    cout << "------------------------------------------" << endl;
    while(1)
    {
        getline(cin,line);
        if(line == ";")
        {
            break;
        }
        text = text + "\n" + line;

    }

    message = "SEND\n"  + convertToString(receiver) + "\n" + subject + text + "\n" + ".\n";
   
    return message;

    
}

string TCPRequest::listMail(){
    string message;
    message = "LIST\n";
    return message;
}

string TCPRequest::readMail(){
    string message;
    int messageNumber;

    cout << "Please enter messagenumber to read:";
    cin >> messageNumber;

    message = "READ\n" + to_string(messageNumber) + "\n";

    return message;
}

string TCPRequest::deleteMail(){
    string message;
    int messageNumber;

    cout << "Please enter messagenumber to delete:";
    cin >> messageNumber;

    message = "DEL\n" + to_string(messageNumber) + "\n";

    return message;
}

string TCPRequest::quitMail(){
    return "QUIT\n";
}

string TCPRequest::loginMail(){
    string message;
    char username[10];
    char password[256];
    

    cout << "Please enter username: ";
    cin >> username;
    username[9] = '\0';

    cout << "Please enter password: ";
    cin >> password;


    message = "LOGIN\n"  + convertToString(username) + "\n" + convertToString(password) + "\n";
   
    return message;
}


//-------------------privat-----------------//

string TCPRequest::convertToString(char * charArray)
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



 

/*string TCPRequest::takePasswdFromUser()
{
    // Stores the password
    string passwd = "";
    char sp = '*'
    char ch_ipt;
 
    // Until condition is true
    while (true) {
 
        ch_ipt = getch();
 
        // if the ch_ipt
        if (ch_ipt == IN::IN_RET) {
            cout << endl;
            return passwd;
        }
        else if (ch_ipt == IN::IN_BACK
                 && passwd.length() != 0) {
            passwd.pop_back();
 
            // Cout statement is very
            // important as it will erase
            // previously printed character
            cout << "\b \b";
 
            continue;
        }
 
        // Without using this, program
        // will crash as \b can't be
        // print in beginning of line
        else if (ch_ipt == IN::IN_BACK
                 && passwd.length() == 0) {
            continue;
        }
 
        passwd.push_back(ch_ipt);
        cout << sp;
    }
}*/