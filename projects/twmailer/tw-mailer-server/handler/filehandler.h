#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


using namespace std;


string tolowers(string tolower)
{

   for_each(tolower.begin(), tolower.end(), [](char &c)
                 { c = ::tolower(c); });
   return tolower;
}



string create_user_dir(string username , string spoolpath){

    string dirpath = spoolpath + "/" + username;

     if(mkdir(dirpath.c_str(), 0777) == -1){

         //cerr << " Error : " << strerror(errno) << endl;
         return spoolpath + "/" + username; 

     }
       
     else{

        cout << "File Created";
        return spoolpath + "/" + username;
     }
        

}


string search_for_file(string dirpath, string filename){

   struct dirent *direntp;
   DIR *dirp;
   string filepath;
   string currentfile;

    // test if directory can be opened
   if ((dirp = opendir(dirpath.c_str())) == NULL)
   {
      fprintf(stderr, "Error opening the directory: %s\n", dirpath.c_str());
      return "Error";
   }

   while ((direntp = readdir(dirp)) != NULL)
   {
      if (direntp->d_type == DT_REG)
      {
         

        currentfile = tolowers(direntp->d_name);
        filename = tolowers(filename);
        
        if (currentfile.find(filename) != std::string::npos)
        {
            filepath = dirpath + "/" + direntp->d_name;

            return filepath;
        }
        
      }
   }

   return "Error";
}


int countmessages(string searchpath){

   struct dirent *direntp;
   DIR *dirp;
   int msgcounter = 0;
    // test if directory can be opened
   if ((dirp = opendir(searchpath.c_str())) == NULL)
   {
      fprintf(stderr, "Error opening the directory: %s\n", searchpath.c_str());
      return 0;
   }


   while ((direntp = readdir(dirp)) != NULL)
   {
      if (direntp->d_type == DT_REG)
      {
         
        msgcounter++;
        
      }
   }


    return msgcounter;

}



bool writefile(string sender, string recipient, string subject, stringstream &body,string spoolpath){

    string filepath;
    string line;
   //stringstream stream(body);
    int msgID;
   
    filepath = create_user_dir(recipient,spoolpath);

    msgID = countmessages(filepath);

   if(msgID == -1){

      return false;
   }

   string name = recipient + "_message" + to_string(msgID);

   ofstream file(filepath + "/" + name + ".txt");

    file << sender << "\n";
    file << recipient << "\n";
    file << subject << "\n";

      while (!body.eof()) {
        getline(body, line);

        if (strcmp(line.c_str(), "."))
        {
        file << line + "\n";
        line = "";
        }
       
    }

    file.close();

    return true;

}


string readfile(string filepath){

   string messagetext;
   string temp;


   ifstream file(filepath);

   while (getline(file, temp,'\n')) {
      // Output the text from the file
      cout << temp << endl;
      messagetext += temp + '\n';
   }

   return messagetext;
}



string create_mail_list(string user, string spoolpath){

   struct dirent *direntp;
   DIR *dirp;
   string filepath;
   string path = spoolpath + "/" + user;
   string mailist;
   int messagecounter;
   fstream file;
   string subject;

   if(countmessages(path) == 0){

      return "0";
   }


   mailist = to_string(countmessages(path)) + "\n";

    // test if directory can be opened
   if ((dirp = opendir(path.c_str())) == NULL)
   {
      fprintf(stderr, "Error opening the directory: %s\n", path.c_str());
      return "0";
   }


   while ((direntp = readdir(dirp)) != NULL)
   {
      if (direntp->d_type == DT_REG)
      {  
         file.open(path +"/"+ direntp->d_name);
         getline(file, subject);
         getline(file, subject);
         getline(file, subject);
         mailist = mailist + subject + "\n";
       /* currentfile = (casesensitive) ? direntp->d_name : tolowers(direntp->d_name);
        filename = (casesensitive) ? filename : tolowers(filename);
        
        if (currentfile.find(filename) != std::string::npos)
        {
            filepath = searchpath + "/" + direntp->d_name;
        }*/
        
      }
   }


   return mailist;
}



