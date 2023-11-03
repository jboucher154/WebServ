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

bool Validator::listen( std::string value ){

	(void)value;
	return true;
}

bool Validator::serverName( std::string value ){

	(void)value;
	return true;
}

bool Validator::host( std::string value ){

	(void)value;
	return true;
}

bool Validator::root( std::string value ){

	(void)value;
	return true;
}

bool Validator::clientMaxBodySize( std::string value ){

	(void)value;
	return true;
}

bool Validator::index( std::string value ){

	(void)value;
	return true;
}

bool Validator::errorPage( std::string value ){

	(void)value;
	return true;
}

bool Validator::allowedMethods( std::string value ){

	(void)value;
	return true;
}

bool Validator::autoIndex( std::string value ){

	(void)value;
	return true;
}

bool Validator::returnKey( std::string value ){

	(void)value;
	return true;
}

bool Validator::alias( std::string value ){

	(void)value;
	return true;
}

bool Validator::cgiExt( std::string value ){

	(void)value;
	return true;
}

bool Validator::clientBodyLimit( std::string value ){

	(void)value;
	return true;
}

bool Validator::cgiPath( std::string value ){

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
