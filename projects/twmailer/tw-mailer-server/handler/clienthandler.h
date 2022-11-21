#include <string>
#include <cstring>
#include <iostream>
#include <sys/socket.h>


using namespace std;




string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

string CharToString(char * charToConvert){
    int size = sizeof(charToConvert) / sizeof(char);
    string result = convertToString(charToConvert, size);

    return result;
}




string sendMail(){
    string message;
    string line;
    string text;
    string subject;
    char user[9];
    char receiver[9];
    

    cout << "Please Enter Sender: ";
    cin >> user;

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

    message = "SEND\n" + CharToString(user) + "\n" + CharToString(receiver) + "\n" + subject + text + "\n" + ".\n";
   
    return message;

    
}

string listMail(){
    string message;
    char user[9];
    cout << "Please enter username:";
    cin >> user;

    message = "LIST\n" + CharToString(user) + "\n";

    return message;
}

string readMail(){
    string message;
    char user[9];
    int messageNumber;
    cout << "Please enter username:";
    cin >> user;

    cout << "Please enter messagenumber to read:";
    cin >> messageNumber;

    message = "READ\n" + CharToString(user) + "\n" + to_string(messageNumber) + "\n";

    return message;
}

string deleteMail(){
    string message;
    char user[9];
    int messageNumber;

    cout << "Please enter username:";
    cin >> user;

    cout << "Please enter messagenumber to delete:";
    cin >> messageNumber;

    message = "DEL\n" + CharToString(user) + "\n" + to_string(messageNumber) + "\n";

    return message;
}

string quitMail(){
    return "QUIT\n";
}







bool clientMenu(int cli_socket){
    char input;
    bool state = true;
    cout << "Please select one of the following options:" << endl;
    cout << "(S)end, (L)ist, (R)ead, (D)elete, (Q)uit \n\n" << endl;

    cout << ":>";
    cin >> input;

    string message;

    switch(input)
    {
        case 'S':
        case 's':
            message = sendMail();
            break;

        case 'L':
        case 'l':
            message = listMail();
            break;

        case 'R':
        case 'r':
            message = readMail();
            break;

        case 'D':
        case 'd':
            message = deleteMail();
            break;

        case 'Q':
        case 'q':
            message = quitMail();
            state = false;
            break;

    }

   /* cout << "----------------------------" << endl;
    cout << message << endl;
    cout << "----------------------------" << endl;*/
    send(cli_socket, message.c_str(), strlen(message.c_str()), 0);
    
    return state;
}