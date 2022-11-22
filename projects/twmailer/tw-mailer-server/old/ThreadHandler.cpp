#include "ServerProcessor.cpp"
#include "Logger.cpp"
#include <thread>
#include <vector>


using namespace std;

class ThreadHandler{
    public:
        ThreadHandler()
        {
           
        }

        void addHandler(int client, string spoolpath, string userip, Logger * log)
        {
            processors.push_back(thread(&ThreadHandler::processConnection,this,client,spoolpath,userip,log));
        }

        void controlHandler()
        {
            /*for(int i = 0; i < processors.size(); i++)
            {
                if (processors.at(i).joinable())
                {
                    processors.at(i).join();
                }
            }*/
        }



    private:
        vector<thread> processors;
        

        void processConnection(int client, string spoolpath, string userip, Logger * log)
        {
            ServerProcessor processor = ServerProcessor(client, spoolpath, userip, log);
            processor.run();
        }
};