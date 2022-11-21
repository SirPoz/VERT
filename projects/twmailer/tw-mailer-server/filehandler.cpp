#pragma once
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctime>
#include <vector>
#include "email.cpp"

using namespace std;

class filehandler
{
public:
   string tolowers(string tolower)
   {
      for_each(tolower.begin(), tolower.end(), [](char &c)
               { c = ::tolower(c); });
      return tolower;
   }

   void writeblacklistentry(string ipadress)
   {

      string blacklist;
      time_t rawtime;
      struct tm *timeinfo;
      char buffer[80];

      time(&rawtime);
      timeinfo = localtime(&rawtime);

      strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
      string str(buffer);

      blacklist = ipadress + "_" + str + "\n";

      ofstream file("blacklist.txt", ofstream::app);

      file << blacklist;

      file.close();
   }

   string readblacklistentry(string ipadress)
   {

      string temp;
      string blacklistentry;

      ifstream file("blacklist.txt");

      if (file.is_open() == 1)
      {

         while (file)
         {
            getline(file, temp);

            if (temp.find(ipadress) != std::string::npos)
            {
               blacklistentry = temp;
            }
         }
      }
      else
      {

         return "Err\n";
      }
      file.close();

      return blacklistentry;
   }

   void deleteblacklistentry(string ipadress)
   {

      string temp;
      vector<string> lines;
      string line;

      cout << line << endl;

      ifstream readfile;

      readfile.open("blacklist.txt");

      while (getline(readfile, line))
      {
         lines.push_back(line);
      }

      readfile.close();

      ofstream writefile;

      writefile.open("blacklist.txt");

      for (int i = 0; i < lines.size(); i++)
      {
         if (!(lines[i].find(ipadress) != std::string::npos))
         {
            writefile << lines[i] << endl;
         }
      }
      writefile.close();
      lines.clear();
   }

   string create_user_dir(string username, string spoolpath)
   {

      string dirpath = spoolpath + "/" + username;

      string mailspath = spoolpath + "/" + username + "/mails";

      mkdir(dirpath.c_str(), 0777);
      mkdir(mailspath.c_str(), 0777);

      return spoolpath + "/" + username;
   }

   string search_for_file(string dirpath, int line)
   {
      struct dirent *direntp;
      DIR *dirp;
      string filepath;
      string currentfile;
      int counter = 1;

      // test if directory can be opened
      if ((dirp = opendir(dirpath.c_str())) == NULL)
      {
         fprintf(stderr, "Error opening the directory: %s\n", dirpath.c_str());
         return "Err\n";
      }

      while ((direntp = readdir(dirp)) != NULL)
      {
         if (direntp->d_type == DT_REG)
         {
            if (counter == line)
            {
               filepath = dirpath + "/" + direntp->d_name;

               return filepath;
            }
            counter++;
         }
      }

      return "Err\n";
   }

   int countmessages(string searchpath)
   {

      struct dirent *direntp;
      DIR *dirp;
      int msgcounter = 1;
      string temp;
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

   void writemailsubjectlist(string subject, string username, string path, string MsgID)
   {

      ofstream file(path + "/mails_" + username + ".txt", ofstream::app);
      file << subject << endl;
      file.close();
   }

   bool writemailfile(email *mail, string spoolpath)
   {
      string filepath;
      string line;
      int msgID;
      string userpath;
      stringstream body(mail->body);

      userpath = create_user_dir(mail->receiver, spoolpath);

      filepath = userpath + "/mails";

      msgID = countmessages(filepath);

      if (msgID == -1)
      {

         return "Err\n";
      }

      writemailsubjectlist(mail->subject, mail->receiver, userpath, to_string(msgID));

      string name = mail->id;

      ofstream file(filepath + "/" + name + ".txt");

      file << mail->sender << "\n";
      file << mail->receiver << "\n";
      file << mail->subject << "\n";

      while (!body.eof())
      {
         getline(body, line);

         if (strcmp(line.c_str(), "."))
         {
            file << line + "\n";
            line = "";
         }
      }

      file.close();

      return "Ok\n";
   }

   string readfile(string username, int mailnumber, string spoolpath)
   {

      string messagetext;
      string temp;
      string filepath = spoolpath + "/" + username + "/mails";

      filepath = search_for_file(filepath, mailnumber);

      ifstream file(filepath);

      if (file.is_open() == 1)
      {

         while (file)
         {
            getline(file, temp);
            messagetext += temp + '\n';
         }
         return messagetext;
      }
      else
      {

         return "Err\n";
      }
   }

   int findmsgID(char array[])
   {

      char tosearch = '.';

      for (int n = 0; n < 256; ++n)
      {
         if (array[n] == tosearch)
         {

            return n - 1;
         }
      }
      return 0;
   }

   string create_mail_list(string user, string spoolpath)
   {

      string filepath;
      string path = spoolpath + "/" + user + "/mails_" + user + ".txt";
      string mailist;
      string mailisttemp;
      string temp;
      fstream file;
      vector<string> lines;

      file.open(path);


      while (getline(file, temp))
      {
         lines.push_back(temp);
      }

       for (int i = 0; i < lines.size(); i++)
      {
          mailisttemp += to_string(i+1) + ") " + lines[i] + "\n";
      }
      file.close();

      mailist = to_string(lines.size()) + "\n" + mailisttemp;

      lines.clear();
      return mailist;
   }


   void deletemailistentry(int mailnumber,string filepath)
   {

      string temp;
      vector<string> lines;
      string line;
      int tempnumber = mailnumber;

      cout << line << endl;

      ifstream readfile;

      readfile.open(filepath);

      while (getline(readfile, line))
      {
         lines.push_back(line);
      }

      readfile.close();

      ofstream writefile;

      writefile.open(filepath);

      tempnumber--;

      for (int i = 0; i < lines.size(); i++)
      {
        if (i != tempnumber)
         {
            writefile << lines[i] << endl;
         }
      }
      writefile.close();
      lines.clear();
   }



   string deletefile(string username, int mailnumber, string spoolpath)
   {

      string filepath = spoolpath + "/" + username + "/mails";
      string filepathmaillist = spoolpath + "/" + username + "/mails_" + username + ".txt";
      filepath = search_for_file(filepath, mailnumber);

      deletemailistentry(mailnumber,filepathmaillist);

      cout << filepath << endl;

      if (remove(filepath.c_str()) != 0)
      {

         return "Err\n";
      }
      else
      {
         return "Ok\n";
      }
      return "Err\n";
   }
};