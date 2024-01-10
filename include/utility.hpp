#ifndef UTILITY_HPP
/*! \brief utility header collects prototype of methods that can be used throught the project.
*  
*  Methods collected in this header do not belong together, hence utility not being a class.
*
*/
# define UTILITY_HPP

#include "Logger.hpp"
#include "Color.hpp"

# include <ctime>
# include <time.h>
# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <arpa/inet.h>

int 		ft_stoi(const std::string& str);
std::string	intToString(int integer);
bool		isValidIpAddress(const char* ipAddress);
bool		isFile( std::string& path );
bool		isDirectory( std::string& path );
char**		copyCStringArray( char** array );
char*		ft_strdup( const char* str );
void		deleteAllocatedCStringArray( char** array );
std::string	urlEncode( std::string to_encode );
bool 		isAllDigit(const std::string& s);
bool 		canOpen( std::string& path );
std::string urlDecode(const std::string& input);
std::string	getTimestampString( void );

#endif