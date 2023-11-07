#include "Validator.hpp"

std::string valid_main_keys_array[] = {"listen", "server_name", "host", "root",
		 "client_max_body_size", "index", "error_page"};
std::vector<std::string> valid_main_keys(valid_main_keys_array, valid_main_keys_array
	+ sizeof(valid_main_keys_array) / sizeof(valid_main_keys_array[0]));
std::string valid_location_keys_array[] = {"allow_methods", " autoindex", "index", "return",
		 "alias", "root", "cgi_ext", "client_body_limit", "cgi_path"};
std::vector<std::string> valid_location_keys(valid_location_keys_array, valid_location_keys_array
	+ sizeof(valid_location_keys_array) / sizeof(valid_location_keys_array[0]));


Validator::Validator(){
	
}

Validator::~Validator(){
}

Validator::Validator( const Validator& src ){

	*this = src;
}

Validator& Validator::operator=( const Validator& rhs ){
	
	(void)rhs;
	return *this;
}

bool Validator::listen( std::string& value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for listening port can not be empty!");
		return false;
	}
	return true;
}

bool Validator::serverName( std::string& value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for server name can not be empty!");
		return false;
	}
	if( value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789.")
		!= std::string::npos ){
		Logger::log(E_ERROR, COLOR_RED, "Server name can not have chatacters other than digits alphabetical characters, undescore and dot!");
		return false;
	}
	return true;
}

bool Validator::host( std::string& value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for host can not be empty!");
		return false;
	}
	if ( value.compare("localhost") == 0 || value.compare("::1") == 0
		|| value.compare("127.0.0.1") == 0){
		Logger::log(E_ERROR, COLOR_RED, "Server can not be hosted at %s", value);
		//*value = "127.0.0.1";
		return true;
		}
	//check if there are 3 dots
	size_t first_dot = value.find(".");
	size_t second_dot = value.find(".", first_dot + 1 );
	size_t third_dot = value.find(".", second_dot + 1);
	size_t forth_dot = value.find(".", third_dot + 1);
	if ( first_dot == std::string::npos || second_dot == std::string::npos
		|| third_dot == std::string::npos || forth_dot != std::string::npos ){
		Logger::log(E_ERROR, COLOR_RED, "The field for host has invalid value!");
		return false;
	}
	//check if the value is only numbers beside the dots
	if( value.find_first_not_of("0123456789.") != std::string::npos ){
		Logger::log(E_ERROR, COLOR_RED, "Server name can not have chatacters other than digits alphabetical characters, undescore and dot!");
		return false;
	}
	//reads the values before dots into a steing
	// ft_stoi() the string into an int
	//the int should be between 0 to 255
	return true;
}

bool Validator::root( std::string& value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for host can not be empty!");
		return false;
	}

	return true;
}

bool Validator::clientMaxBodySize( std::string& value ){

	(void)value;
	return true;
}

bool Validator::index( std::string& value ){

	(void)value;
	return true;
}

bool Validator::errorPage( std::string& value ){

	(void)value;
	return true;
}

bool Validator::allowedMethods( std::string& value ){

	(void)value;
	return true;
}

bool Validator::autoIndex( std::string& value ){

	(void)value;
	return true;
}

bool Validator::returnKey( std::string& value ){

	(void)value;
	return true;
}

bool Validator::alias( std::string& value ){

	(void)value;
	return true;
}

bool Validator::cgiExt( std::string& value ){

	(void)value;
	return true;
}

bool Validator::clientBodyLimit( std::string& value ){

	(void)value;
	return true;
}

bool Validator::cgiPath( std::string& value ){

	(void)value;
	return true;
}

bool Validator::validate(std::string	infile){
	// t_main_block_functs  mainFunct[] = { &Validator::listen, &Validator::serverName, &Validator::host, &Validator::root,
	// 			&Validator::clientMaxBodySize, &Validator::index, &Validator::errorPage};
	// t_location_block_functs  locationFunct[] = { &Validator::allowedMethods, &Validator::autoIndex, &Validator::returnKey,
	// 			&Validator::alias, &Validator::cgiExt, &Validator::clientBodyLimit, &Validator::cgiPath};
	(void)infile;

	return true;
}
