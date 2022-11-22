#pragma once
#include <iostream>

using namespace std;

class Logger{
    public:
        Logger(){
           
        }

        void LogError(string log){
           
            cout << error << log << endl;
           
        }
        void LogWarning(string log){
            cout << warning << log <<endl;
           
        }

        void LogSuccess(string log){
            cout << success << log << endl;
        
        }

        void LogInfo(string log){
            cout << info << log << endl;
         
        }

        void Log(string log){
            cout << normal << log << endl;
            
        }

        string red(string text)
        {
            return "\x1B[31m" + text + "\033[0m";
        }

        string yellow(string text)
        {
            return "\x1B[33m" + text + "\033[0m";
        }

        string green(string text)
        {
            return "\x1B[32m" + text + "\033[0m";
        }

        string blue(string text)
        {
            return "\x1B[34m" + text + "\033[0m";
        }
    
    private:
        string error = "[\x1B[31mERRO\033[0m] ";
        string success = "[\x1B[32mOKAY\033[0m] ";
        string warning = "[\x1B[33mWARN\033[0m] ";
        string info = "[\x1B[34mINFO\033[0m] ";
        string normal =  "[\x1B[37mLOG \033[0m] ";
        pthread_mutex_t * logMutex;
 
};