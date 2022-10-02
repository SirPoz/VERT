#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>

using namespace std;

string tolowers(string tolower)
{

   for_each(tolower.begin(), tolower.end(), [](char &c)
                 { c = ::tolower(c); });
   return tolower;
}

void searchfile(string searchpath, string filename, bool recursive, bool casesensitive)
{

   struct dirent *direntp;
   DIR *dirp;
   string newsearchpath;
   string filepath;
   string currentfile;

   if ((dirp = opendir(searchpath.c_str())) == NULL)
   {
      //perror("Failed to open directory");
      return;
   }

   while ((direntp = readdir(dirp)) != NULL)
   {
      if (direntp->d_type == 8)
      {
         

         currentfile = (casesensitive) ? direntp->d_name : tolowers(direntp->d_name);
         filename = (casesensitive) ? filename : tolowers(filename);
        
         if (currentfile.find(filename) != std::string::npos)
         {
            filepath = searchpath + "/" + direntp->d_name;
            cout << filepath << endl;
         }
        
      }
      if (direntp->d_type == 4)
      {

         if (recursive)
         {
            // strcmp returns false when both strings are equal
            if (strcmp(direntp->d_name, "..") && strcmp(direntp->d_name, "."))
            {
               newsearchpath = searchpath + "/" + direntp->d_name;
               
               searchfile(newsearchpath, filename, recursive, casesensitive);
            }
         }
      }
   }
}


