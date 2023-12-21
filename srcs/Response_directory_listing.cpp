#include "Response.hpp"
#include <cstring>
#include <cstdlib>
#include <dirent.h>


void listFiles(const std::string& path, std::vector<std::string>& directoryVec)
{

    const char pathSeparator = '/';
  
    DIR                         *dir;
    struct dirent               *ent;
    std::string                 element;

    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            const std::string filename = ent->d_name;

            if (filename != "." && filename != "..")
            {
                const std::string element = path + pathSeparator + filename;
                directoryVec.push_back(element);

                if (ent->d_type == DT_DIR)
                {
                    // Recursively call listFiles for subdirectories
                    listFiles(path + "/" + filename, directoryVec);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "Error opening directory" << std::endl;
    }
}

std::string buildHtmlList(const std::string& path, const std::vector<std::string>& directoryVec)
{
    std::string htmlString = "<html>\n\t<head>\n\t</head>\n\t<body>\n\t\t<ul>\n";

    for (size_t i = 0; i < directoryVec.size(); ++i)
    {
        htmlString += "\t\t\t<li><a href=\"" + directoryVec[i] + "\">" + directoryVec[i].substr(path.size() + 1) + "</a></li>\n";
    }

    htmlString += "\n\t\t</ul>\n\t</body>\n</html>";

    return htmlString;
}
