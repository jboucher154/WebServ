# include <sys/stat.h>
#include "utility.hpp"

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