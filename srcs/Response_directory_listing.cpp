#include "Response.hpp"
#include <cstring>
#include <cstdlib>
#include <dirent.h>


void Response::listFiles(const std::string& path, std::vector<std::string>& directoryVec)
{
  
    DIR                         *dir;
    struct dirent               *ent;
    std::string                 element;

    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            const std::string filename = ent->d_name;
            if (filename.front() != '.')
            {
                const std::string element = path + "/" + filename;
                if (access( element.c_str(), R_OK) == 0)
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
        this->status_code_ = 403;
		Logger::log(E_DEBUG, COLOR_CYAN, "403 read access not allowed for resource file: `%s'", this->request_->getRequestLineValue("uri").c_str());
    }
}

std::string Response::buildHtmlList(const std::string& path)
{

    int cut_size = 0;
    std::string location_path = "";
    std::vector<std::string> directoryVec;
    std::cout << "path: " << path << std::endl;
    if (this->resource_location_ == "/"){
        listFiles(path.substr(0, path.size() - 1), directoryVec);
        cut_size = path.size();
    }
    else{
        listFiles(path, directoryVec);
        cut_size = path.size() + 1;
        location_path = this->resource_location_ + "/";
    }
    std::string htmlString = "<html>\n\t<head>\n\t</head>\n\t<body>\n\t\t<ul>\n";
    if (directoryVec.empty()){
         htmlString += "\t\t\t<li><p>this directory is empty</p></li>\n";
    }
    else{
        for (size_t i = 0; i < directoryVec.size(); ++i)
        {
            htmlString += "\t\t\t<li><a href=\"" + location_path + directoryVec[i].substr(cut_size) +  "\">" + directoryVec[i].substr(cut_size) + "</a></li>\n";
            std::cout << "directoryVec[i]: " << directoryVec[i] << std::endl;
        }
    }
    htmlString += "\n\t\t</ul>\n\t</body>\n</html>";
    return htmlString;
}
