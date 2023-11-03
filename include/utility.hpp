#ifndef UTILITY_HPP
# define UTILITY_HPP

# include <iostream>
# include <string>
# include <sstream>

int 		ft_stoi(const std::string& str);
std::string	int_to_string(int integer);
bool		isFile( std::string& path );
bool		isDirectory( std::string& path );

#endif