#include <sys/stat.h>
#include "utility.hpp"
#include <cstdio>

/*! \brief returns bool indicating if path passed is for a directory.
*       
*	Uses stat struct and S_ISDIR macro to determine if path passed is  directory.
*
*/
bool	isDirectory( std::string& path ) {

	struct stat	file_info;

	if (stat(path.c_str(), &file_info) != 0) {
		return false;
	}
	return S_ISDIR(file_info.st_mode);
}

/*! \brief returns bool indicating if path passed is for a file
*       
*	Uses stat struct and S_ISREG macro to determine if path passed is file.
*
*/
bool	isFile( std::string& path ) {

	struct stat	file_info;

	if (stat(path.c_str(), &file_info) != 0) {
		return false;
	}
	return S_ISREG(file_info.st_mode);
}

/*! \brief attempts to open a file and returns true if successful, closes file after check.
*       
*	Attempts to open a file and returns true if successful, closes file after check.
*	returns false if open fails.
*
*/
bool canOpen( std::string& path ){
	
	FILE* file = std::fopen(path.c_str(), "r");
    if (file != 0) {
        std::fclose(file);
		return true;
    } else {
        return false;
    }
}