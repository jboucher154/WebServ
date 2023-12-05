#ifndef VALIDATOR_HPP
# define VALIDATOR_HPP

# include "Logger.hpp"
# include "Color.hpp"
# include "Server.hpp"
# include <iostream>
# include <vector>
# include <string>
# include <map>
# include <cstring>
# include <netdb.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fstream>
# include <sstream>
# include <limits>
# include "utility.hpp"

class Validator
{
private:
	static std::map<std::string, std::string>				validIpHostMap;
	static std::vector<std::string>							lines;
	static size_t 											serverLines;
	static std::string 										mainRootPath;
	static std::string 										rootPath;
	static std::map<std::string, std::vector<std::string> >	innerBlock;

	static std::vector<std::string> valid_main_keys_;
	static std::string valid_main_keys_array;
	static std::vector<std::string> valid_location_keys_;
	static std::string valid_location_keys_array;
	static bool listen( std::string value );
	static bool serverName( std::string value );
	static bool host( std::string value );
	static bool root( std::string value );
	static bool clientMaxBodySize( std::string value );
	static bool index( std::string value );
	static bool errorPage( std::string value, std::string key );
	static bool allowedMethods( std::string value );
	static bool autoIndex( std::string value );
	static bool returnKey( std::string value );
	static bool alias( std::string value );
	static bool cgiExt( std::string value );
	static bool locationIndex( std::string value );
	static bool locationRoot( std::string value );
	static bool cgiPath( std::string value );
	static bool store_lines(std::string	input);
	static bool checkMainBlock(std::vector<std::string>*	lines);
	static bool validate_lines(std::vector<std::string>*	lines);
	static bool validate_server(std::vector<std::string>*	lines);
	static bool storeInnerBlock(std::vector<std::string>*	lines, size_t i);
	static bool checkMainBlockKeyValues(void);
	static bool checkLocationBlock(std::vector<std::string>*	lines);
	static bool checkBraces(std::vector<std::string>*	lines);
	static size_t countServerLines(std::vector<std::string>*	lines);
	static bool checkLocationBlockKeyValues(std::string	locationKey);
	static bool checkCgiBlockKeyValues();
	static bool validIpHostBuilder();

	Validator();
	Validator( const Validator& src );
	Validator& operator=( const Validator& rhs );
	~Validator();
	
public:

	static std::vector<Server>			servers;
	static bool validate( std::string	infile );
	
};

typedef bool (*t_main_block_functs) ( std::string value );
typedef bool (*t_location_block_functs) ( std::string value );

#endif
