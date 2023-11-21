#ifndef UTILITY_HPP
# define UTILITY_HPP

#include "Logger.hpp"
#include "Color.hpp"

# include <iostream>
# include <string>
# include <sstream>
# include <arpa/inet.h>

int 		ft_stoi(const std::string& str);
std::string	intToString(int integer);
bool		isValidIpAddress(const char* ipAddress);
bool		isFile( std::string& path );
bool		isDirectory( std::string& path );
bool 		isAllDigit(const std::string& s);

#endif