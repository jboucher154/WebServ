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
std::map<std::string, std::vector<std::string> >	Validator::innerBlock;
std::vector<Server>			servers;

/*! \brief Validator's class constructor
*       
*  Doesn't do anything just constructs an instance of the class. 
*  This constructor is private.
*/
Validator::Validator(){
	
}

/*! \brief Validator's class destructor
*       
*  Doesn't do anything just destructs an instance of the class. 
*  This destructor is private.
*/
Validator::~Validator(){
}

/*! \brief Validator's class copy constructor
*       
*  This copy constructor calls on the operator overload to copy the instance. 
*  This copy constructor is private.
*/
Validator::Validator( const Validator& src ){

	*this = src;
}

/*! \brief Validator's assignment operator overload
*       
*  Returns a pointer to the current instance. 
*  This assignment operator overload is private.
*/
Validator& Validator::operator=( const Validator& rhs ){
	
	(void)rhs;
	return *this;
}

/*! \brief validates listening port's value
*       
*  Checks if listening port has a value and that it is 
*  a number and that number is an int and that it is within the valid range.
*/
bool Validator::listen( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for listening port can not be empty!");
		return false;
	}
	if (!isAllDigit(value)){
		Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number!");
		return false;
	}
	try{
		std::cout << ft_stoi(value) << std::endl;
		if (ft_stoi(value) < 1024 || ft_stoi(value) > 65535 ){
			Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number between 1025 and 65535!");
			return false;
		}
	}
	catch(std::exception &e){
		Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number between 1025 and 65535!");
			return false;
	}
	return true;
	
}

/*! \brief validates server name value
*       
*  Checks if listening port has a value and that it is
*  a string consist of only ascii chatacters, digits, undescore and dot.
*/
bool Validator::serverName( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for server name vallue can not be empty!");
		return false;
	}
	if( value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789.")
		!= std::string::npos ){
		Logger::log(E_ERROR, COLOR_RED, "Server name can not have chatacters other than digits alphabetical characters, undescore and dot!");
		return false;
	}
	return true;
}

/*! \brief validates host value
*       
*  Checks if host has a value and then calls on is valid ip address
*  to check if the value is a valid ip address.
*/
bool Validator::host( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for host value can not be empty!");
		return false;
	}
	if (!isValidIpAddress(value.c_str())){
		Logger::log(E_ERROR, COLOR_RED, "Host has to have a valid IP as it's value!");
		return false;
	}
	return true;
}

/*! \brief validates root value
*       
*  Checks if root has a value and that it is a directory
*/
bool Validator::root( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for root value can not be empty!");
		return false;
	}
	if (!isDirectory(value)){
		Logger::log(E_ERROR, COLOR_RED, "Root has to be an existing directory!");
		return false;
	}
	return true;
}

/*! \brief validates client max body size's value
*       
*  Checks if client max body size has a value and that it is 
*  a number and that number is an int and that it is within the valid range.
*/
bool Validator::clientMaxBodySize( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for client max body size value can not be empty!");
		return false;
	}
	if (!isAllDigit(value)){
		Logger::log(E_ERROR, COLOR_RED, "Client max body size value has to be a number!");
		return false;
	}
	try{
		if (ft_stoi(value) < 0 || ft_stoi(value) > 2147483647 ){
			Logger::log(E_ERROR, COLOR_RED, "Client max body size value has to be a number between 1025 and 65535!");
			return false;
		}
	}
	catch(std::exception &e){
		Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number between 1025 and 65535!");
			return false;
	}
	return true;
}

bool Validator::index( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for index value can not be empty!");
		return false;
	}
	std::cout << value << std::endl;
	if (!isFile(value)){
		Logger::log(E_ERROR, COLOR_RED, "Index has to be an existing file!");
		return false;
	}
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

/*! \brief checks the main block
*       
*
*  If innerBlock is not empty loops through and earase all. 
*  reading elements from opening braces on, which we pass the index for
*  till the closing braces,first checks that line ends in semicolon and
*  has key and value seperated by a space. Then splits the lines with spaces and saves
*  lines into the mainBlock map. 
*  When innerBlock is saved in inner block map, removes the coresponding lines from lines
*/
bool  Validator::storeInnerBlock(std::vector<std::string>*	lines, int serverLines, int i){

	//if innerBlock is not empty loop through and earases all 
	while (!innerBlock.empty())
		innerBlock.erase(innerBlock.begin());
		
    std::string key;
    std::string value;
	std::string	root = "";

	while ((*lines)[i] != lines->back() && (*lines)[i].compare("}") != 0 && i != serverLines){
			//first checks that line ends in semicolon and has key and value seperated by a space 
			if( (*lines)[i].find_last_of(';') != (*lines)[i].size() - 1){
				Logger::log(E_ERROR, COLOR_RED, "Ending of each line of the main block should be marked by a semicolon!");
				return false;
			}
			if( (*lines)[i].find(' ') == std::string::npos){
				Logger::log(E_ERROR, COLOR_RED, "Each line of the main block should have a key and values seperated by a space");
				return false;
			}
			std::vector<std::string>							values;
			std::stringstream    line((*lines)[i]);
       		if ((std::getline(line, key, ' ') && std::getline(line, value, ';'))){
				std::stringstream    valuesVec(value);
				while ( std::getline(valuesVec, value, ' ' )){
					values.push_back(value);
				}
				if (key.compare("root"))
					root = values[0]; 
				if ( innerBlock.find(key) == innerBlock.end() )
					innerBlock[key] = values;
				else{
					Logger::log(E_ERROR, COLOR_RED, "repetative keys are not allowed in inner Blocks of the server!");
					return false;
				}
			}
			else{
				Logger::log(E_ERROR, COLOR_RED, "Each line of the main block should have a key and values seperated by a space.");
				return false;
			}
			i++;
	}
	//adds root directory to the begining of the index file
	if ( innerBlock.find("index") != innerBlock.end() ){
		root.append(innerBlock.find("index")->second[0]);
		innerBlock.find("index")->second[0] = root;
	}
	//when innerBlock is saved in inner block map removes the coresponding lines from lines 
	while (i >= 0){
		lines->erase(lines->begin());
		i--;
	}

	return true;
}

static bool checkLocationBlock(std::vector<std::string>*	lines, int serverLines){
	(void) lines;
	(void) serverLines;
	return true;
}

/*! \brief checks the main block key and value
*       
*
*  loops through the keys and ckecks if the key is a valid key.
*  Then calls the coresponding validator for the key if it is a valid one.
*  returns true if all the values for all the keys are valid and false otherwise.
*/
bool Validator::checkMainBlockKeyValues(void){

	t_main_block_functs  mainFunct[] = { &Validator::listen, &Validator::serverName, &Validator::host, &Validator::root,
				&Validator::clientMaxBodySize, &Validator::index, &Validator::errorPage};
	// if (!lines.empty()){

	// 	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++){
	// 		std::cout << *it << std::endl;
	// 	}
	// }

	for (std::map<std::string, std::vector<std::string> >::iterator outerIt = innerBlock.begin(); outerIt != innerBlock.end(); outerIt++){
		//std::cout << "key : " << outerIt->first << std::endl;
		//std::cout << "value : " << outerIt->second[0] << std::endl;
		int i = 0;
		while (i < 7 && valid_main_keys[i].compare(outerIt->first))
			i++ ;
		if (i == 7){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid key.", (*outerIt).first.c_str());
			return false;
		}
		for (std::vector<std::string>::iterator innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
			//std::cout << "value : " << *innerIt << std::endl;
			if (!mainFunct[i](*innerIt)){
				Logger::log(E_ERROR, COLOR_RED, "%s is not a valid value.", (*innerIt).c_str());
				return false;
			}
		}
	}
	return true;
}

/*! \brief checks the main block
*       
*
*  checks that the third element of config file aka the third line says main
*  then checks if the forth element aka the forth line says "{" and starts
*  reading elements from then on till the closing braces, spliting and saving
*  lines into the mainBlock map. 
*  
*   
*/
bool Validator::checkMainBlock(std::vector<std::string>*	lines, int serverLines){

	if ((*lines)[2] == lines->back() || (*lines)[2].compare("main") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Server block has to begin with a main block!");
		return false;
	}

	if ((*lines)[3] == lines->back() || (*lines)[3].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Main block should be enclosed in curly braces!");
		return false;
	}
	if (!storeInnerBlock(lines, serverLines, 4))
		return false;

	if (!innerBlock.empty()) {
		return (checkMainBlockKeyValues());
	}
	else{
		Logger::log(E_ERROR, COLOR_RED, "Main block can not be empty!");
		return false;
	}
	return true;
}


/*! \brief validates one server
*       
*
*  checks if the braces macht for this server block, then calls on
*  main block validator and then location block validators if they
*  return true then this server block is valid, otherwise it' not. 
*/
bool Validator::validate_server(std::vector<std::string>*	lines, int serverLines){
	if (!checkBraces(lines, serverLines))
		return false;
	if (!checkMainBlock(lines, serverLines))
		return false;
	if (!checkLocationBlock(lines, serverLines))
		return false;
	return true;
}

/*! \brief validates server blocks one by one
*       
*
*  checks that the first element aka first line of the config file says server
*  calls a function that returns how many lines there are in this server block
*  calls a function that validates this server block and if valid pops the 
*  elements from lines vector. If this block is valid and there are more 
*  elements left in lines it calls itself to validate the rest of the config file. 
*/
bool Validator::validate_lines(std::vector<std::string>*	lines){
	if ( lines->front().compare("server") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Config file should start with a server block!");
		return false;
	}
	int serverLines = getServerLines(lines);
	if (!validate_server(lines, serverLines))
		return false;
	// if ((*lines)[serverLines] != lines->back())
	// 	return (validate_lines(lines));
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

	// t_location_block_functs  locationFunct[] = { &Validator::allowedMethods, &Validator::autoIndex, &Validator::returnKey,
	// 			&Validator::alias, &Validator::cgiExt, &Validator::clientBodyLimit, &Validator::cgiPath};

	if ( !store_lines(input) || lines.empty()){
		Logger::log(E_ERROR, COLOR_RED, "The config file is empty!");
		return false;
	}
	// if (!lines.empty()){

	// 	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++){
	// 		std::cout << *it << std::endl;
	// 	}
	// }
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
