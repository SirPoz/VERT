#pragma once
#include <string>
#include <string.h>
#include <iostream>
#include <cstring>
#include <chrono>

using namespace std;

class email{
        

    public:
        string sender;
        string receiver;
        string subject;
        string body;
        string id;
        email(string send, string rec, string subj, string bod)
        {
            this->sender = send;
            this->receiver = rec;
            this->subject = subj;
            this->body = bod;
            this->id = this->hash(send);
        }

    private:

        string hash(string sender)
        {
            const auto p1 = chrono::system_clock::now();
            unsigned long timestamp = chrono::duration_cast<chrono::milliseconds>(p1.time_since_epoch()).count();
            unsigned int date = (int)timestamp/1000000;
            unsigned int time = (int)timestamp - (date*1000000);


            return to_string(date) + sender + to_string(time);

        }

        
};