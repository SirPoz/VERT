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












