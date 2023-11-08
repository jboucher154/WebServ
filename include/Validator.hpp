#ifndef VALIDATOR_HPP
# define VALIDATOR_HPP

# include <iostream>
# include <vector>
# include <string>
# include "Logger.hpp"
# include "Color.hpp"
# include <fstream>
# include <sstream>
# include <limits>

class Validator
{
private:
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
	static bool errorPage( std::string value );
	static bool allowedMethods( std::string value );
	static bool autoIndex( std::string value );
	static bool returnKey( std::string value );
	static bool alias( std::string value );
	static bool cgiExt( std::string value );
	static bool clientBodyLimit( std::string value );
	static bool cgiPath( std::string value );
	
public:
	Validator();
	Validator( const Validator& src );
	Validator& operator=( const Validator& rhs );
	~Validator();

	static bool validate( std::string	infile );
	
};

typedef bool (*t_main_block_functs) ( std::string value );
typedef bool (*t_location_block_functs) ( std::string value );

#endif
