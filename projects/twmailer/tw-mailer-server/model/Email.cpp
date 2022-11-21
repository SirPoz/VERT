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
            this->id = this->hash(rec,subj);
        }

    private:

        string hash(string receiver, string subject)
        {
            const auto p1 = system_clock::now();
            unsigned long time = duration_cast<milliseconds>(p1.time_since_epoch()).count();

            

        }
}