#include <sys/stat.h>
#include "utility.hpp"
#include <cstdio>

bool	isDirectory( std::string& path ) {

	struct stat	file_info;

	if (stat(path.c_str(), &file_info) != 0) {
		return (false);
	}
	return (S_ISDIR(file_info.st_mode));
}

bool	isFile( std::string& path ) {

	struct stat	file_info;

	if (stat(path.c_str(), &file_info) != 0) {
		return (false);
	}
	return (S_ISREG(file_info.st_mode));
}

bool canOpen( std::string& path ){
	
	FILE* file = std::fopen(path.c_str(), "r");
    if (file != 0) {
        std::fclose(file);
		return true;
    } else {
        return false;
    }
}