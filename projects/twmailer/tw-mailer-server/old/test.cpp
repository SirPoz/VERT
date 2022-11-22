#include <iostream>
#include <stdio.h>
#include <chrono>
#include <unistd.h>
using namespace std;

int main(int argc, char **argv){
    for(int i = 0; i < 10; i++)
    {
        const auto p1 = chrono::system_clock::now();
        unsigned long seconds = chrono::duration_cast<chrono::seconds>(p1.time_since_epoch()).count();
        unsigned long milli = chrono::duration_cast<chrono::milliseconds>(p1.time_since_epoch()).count();
        milli = milli/(60000*60*24);
        milli = milli/365;
        printf("%d: %ld\n",i,milli);
        usleep(10000);
    }
    
    return 0;
}