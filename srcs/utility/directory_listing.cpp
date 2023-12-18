#include "utility.hpp"
#include <cstring>
#include <cstdlib>
#include <dirent.h>


std::vector<std::string>* listFiles(const std::string& path)
{

    const char pathSeparator = '/';
  
    DIR                         *dir;
    struct dirent               *ent;
    std::vector<std::string>*	directoryVec = new std::vector<std::string>();
    std::string                 element;

    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            const std::string filename = ent->d_name;

            if (filename != "." && filename != "..")
            {
                element = path + pathSeparator + filename;
                directoryVec->push_back(element);
                if (ent->d_type == DT_DIR)
                {
                    // Recursively call listFiles for subdirectories
                    listFiles(path + "/" + filename);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "Error opening directory" << std::endl;
    }
    return directoryVec;
}