#include "Validator.hpp"

std::string valid_main_keys_array[] = {"listen", "server_name", "host", "root",
		 "client_max_body_size", "index", "error_page"};
std::vector<std::string> valid_main_keys(valid_main_keys_array, valid_main_keys_array
	+ sizeof(valid_main_keys_array) / sizeof(valid_main_keys_array[0]));
std::string valid_location_keys_array[] = {"allow_methods", " autoindex", "index", "return",
		 "alias", "root", "cgi_ext", "client_body_limit", "cgi_path"};
std::vector<std::string> valid_location_keys(valid_location_keys_array, valid_location_keys_array
	+ sizeof(valid_location_keys_array) / sizeof(valid_location_keys_array[0]));

std::string					Validator::string = "";
std::vector<std::string>	Validator::lines;
std::vector<Server>			servers;

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

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for listening port can not be empty!");
		return false;
	}
	return true;
}

bool Validator::serverName( std::string value ){

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

bool Validator::host( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for host can not be empty!");
		return false;
	}
	
	return (isValidIpAddress(value.c_str()));
}

bool Validator::root( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for host can not be empty!");
		return false;
	}

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


/*! \brief returns the number of elements aka lines in the server block
*       
*
*  counts how many lines there are in this server block up until the
*  next server block or the end. 
*/
static int getServerLines(std::vector<std::string>*	lines){
	int serverLines = 1;
	while ( (*lines)[serverLines] != lines->back() && (*lines)[serverLines].compare("server") == 0 ){
		serverLines++;
	}
	return serverLines;
}

/*! \brief checks if all open braces are closed
*       
*
*  checks that the first element aka first line of the server block says {
*  checks that the open braces are closed. checks that no more than 2 braces are
*  left open at any point. and checks that there are not more close braces than open ones.
*/
static bool checkBraces(std::vector<std::string>*	lines, int serverLines){
	int i = 1;
	if ((*lines)[i] == lines->back() || (*lines)[i].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Server block should be enclosed in curly braces!");
		return false;
	}
	int openBraces = 1;
	while (i <= serverLines ){
		if ((*lines)[serverLines].compare("{") != 0){
			openBraces++;
		}
		else if ((*lines)[serverLines].compare("}") != 0){
			openBraces--;
		}
		if (openBraces < 0){
			Logger::log(E_ERROR, COLOR_RED, "Opening curly braces don't match closing ones!");
		return false;
		}
		if (openBraces > 2){
			Logger::log(E_ERROR, COLOR_RED, "There could only be 2 layers of blocks in config file\
			, the server block and the location block");
		return false;
		}
		if (openBraces == 0)
			break;
		i++;
	}
	if (openBraces != 0){
		Logger::log(E_ERROR, COLOR_RED, "Opening curly braces don't match closing ones!");
		return false;
	}
	if (i != serverLines){
		Logger::log(E_ERROR, COLOR_RED, "There could not be anything in between servr blocks!");
		return false;
	}
	return true;
}

static bool checkLocationBlock(std::vector<std::string>*	lines, int serverLines){
	(void) lines;
	(void) serverLines;
	return true;
}

static bool checkMainBlock(std::vector<std::string>*	lines, int serverLines){
	int i = 2;
	if ((*lines)[i] == lines->back() || (*lines)[i].compare("main") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Server block has to have a main block!");
		return false;
	}
	i++;
	if ((*lines)[i] == lines->back() || (*lines)[i].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Main block should be enclosed in curly braces!");
		return false;
	}
	i++;
	std::map<std::string, std::vector<std::string> >	mainBlock;
	std::cout << servers.size() << std::endl;
	while ((*lines)[i] != lines->back() && (*lines)[i].compare("}") != 0 && i != serverLines){
			//store in the main block
			mainBlock.pushback()
			i++;
	}
	return true;
}

static bool validate_server(std::vector<std::string>*	lines, int serverLines){
	if (!checkBraces(lines, serverLines))
		return false;
	checkMainBlock(lines, serverLines);

	checkLocationBlock(lines, serverLines);
	return true;
}

/*! \brief validates server blocks one by one
*       
*
*  checks that the first element aka first line of the config file says server
*  calls a function that returns how many lines there are in this server block
*  calls a function that validates this server block and if valid pups the 
*  elements from lines vector. If this block is valid and there are more 
*  elements left in lines it calls itself to validate the rest of the config file. 
*/
static bool validate_lines(std::vector<std::string>*	lines){
	if ( lines->front().compare("server") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Config file should start with a server block!");
		return false;
	}
	int serverLines = getServerLines(lines);
	if (!validate_server(lines, serverLines))
		return false;
	if ((*lines)[serverLines] != lines->back())
		return (validate_lines(lines));
	return true;
}

/*! \brief reads and stores the config file
*       
*
*  reads and stores the config file into a sring and a vector of strings
*/
bool Validator::store_lines(std::string	input){

	std::ifstream				infile;
	std::string					line;
	std::istringstream 			ss;

	infile.open(input);
	if(infile.fail() == true){
		Logger::log(E_ERROR, COLOR_RED, "The field for host has invalid value!");
		return false;
	}
	
	while (getline(infile, line)) { 
		if (infile.eof() && line.empty()){
			Logger::log(E_ERROR, COLOR_RED, "The config file is empty!");
			return false;
		}
		ss.clear();
    	ss.str(line);
		size_t startPos = line.find_first_not_of(" \t\n\r");
    	if (startPos != std::string::npos) {
        	line = line.substr(startPos);
    	}
		if( line.empty() || startPos == std::string::npos)
			continue ;
		lines.push_back(line);
		std::string temp;
		while (ss >> temp) {
			string += temp;
			if (!ss.eof()){
				string += " ";
			}
		}
		string += "\n";
		ss.str("");
	}
	return true;
}


/*! \brief validates the config file the config file
*       
*
*  stores the config file, if it is empty returns false
*  if not 
*/
bool Validator::validate(std::string	input){

	// t_main_block_functs  mainFunct[] = { &Validator::listen, &Validator::serverName, &Validator::host, &Validator::root,
	// 			&Validator::clientMaxBodySize, &Validator::index, &Validator::errorPage};
	// t_location_block_functs  locationFunct[] = { &Validator::allowedMethods, &Validator::autoIndex, &Validator::returnKey,
	// 			&Validator::alias, &Validator::cgiExt, &Validator::clientBodyLimit, &Validator::cgiPath};

	if ( !store_lines(input) || lines.empty()){
		Logger::log(E_ERROR, COLOR_RED, "The config file is empty!");
		return false;
	}
	if (!lines.empty()){

		for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++){
			std::cout << *it << std::endl;
		}
	}
	return (validate_lines(&lines));
	//1.check that the first line says server
	// 2.check that the server block is opened and closed (this means that there are no unclosed curly braces)
	//2.1 check the blocks within the server block if block is main send 
	//	to 2.1.1 if location send to 2.1.2 if block is something else reject config file.
	//	In the end check no duplication in the location block. 
	//2.1.1 read the first word is it a valid key? is it's matching value a valid value for that key?
	//	Did the read keys include the mandetory keys for the main block? Did we have any duplicate keys?
	//2.1.2 Basicaly do the same thing as 2.1.1 but the fetch the valid values for the respective location blocks
	//3.check that the next line after the server block says server
	//if there are more lines do step 2. to 3. again
	//if there are more lines 
}
