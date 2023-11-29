#include "Validator.hpp"

std::string valid_main_keys_array[] = {"listen", "server_name", "host", "root",
		 "client_max_body_size", "index", "error_page_404", "error_page_405", "error_page_500"};
std::vector<std::string> valid_main_keys(valid_main_keys_array, valid_main_keys_array
	+ sizeof(valid_main_keys_array) / sizeof(valid_main_keys_array[0]));
std::string valid_location_keys_array[] = {" autoindex", "return",
		 "alias", "client_body_limit", "allow_methods", "root", "index", "cgi_ext", "cgi_path"};
std::vector<std::string> valid_location_keys(valid_location_keys_array, valid_location_keys_array
	+ sizeof(valid_location_keys_array) / sizeof(valid_location_keys_array[0]));

std::string											Validator::string = "";
std::vector<std::string>							Validator::lines;
std::map<std::string, std::vector<std::string> >	Validator::innerBlock;
std::vector<Server>									Validator::servers;
size_t 												Validator::serverLines = 0;

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
*  when listening port is checkd its value is pushed back to its server.
*/
bool Validator::listen( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for listening port value can not be empty!");
		return false;
	}
	if (!isAllDigit(value)){
		Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number!");
		return false;
	}
	try{
		//std::cout << ft_stoi(value) << std::endl;
		if (ft_stoi(value) < 1024 || ft_stoi(value) > 65535 ){
			Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number between 1025 and 65535!");
			return false;
		}
	}
	catch(std::exception &e){
		Logger::log(E_ERROR, COLOR_RED, "Listening port value has to be a number between 1025 and 65535!");
			return false;
	}
	//push back to its serve
	servers[servers.size() - 1].setListeningPort(ft_stoi(value));
	return true;
	
}

/*! \brief validates server name value
*       
*  Checks if listening port has a value and that it is
*  a string consist of only ascii chatacters, digits, undescore and dot.
*  when server name is checkd its value is pushed back to its server.
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
	//push back to its serve
	servers[servers.size() - 1].setServerName(value);
	return true;
}

/*! \brief validates host value
*       
*  Checks if host has a value and then calls on is valid ip address
*  to check if the value is a valid ip address.
*  when host is checkd its value is pushed back to its server.
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
	//push back to its serve
	servers[servers.size() - 1].setHost(value);
	return true;
}

/*! \brief validates root value
*       
*  Checks if root has a value and that it is a directory
*  when root is checkd its value is pushed back to its server.
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
	//push back to its serve
	servers[servers.size() - 1].setRoot(value);
	return true;
}

/*! \brief validates client max body size's value
*       
*  Checks if client max body size has a value and that it is 
*  a number and that number is an int and that it is within the valid range.
*  when client max body size is checkd its value is pushed back to its server.
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
		if (ft_stoi(value) < 0 || ft_stoi(value) > INT_MAX ){ 
			Logger::log(E_ERROR, COLOR_RED, "Client max body size value has to be a less than max int!");
			return false;
		}
	}
	catch(std::exception &e){
		Logger::log(E_ERROR, COLOR_RED, "Client max body size value has to be a less than max int!");
			return false;
	}
	//push back to its serve
	servers[servers.size() - 1].setClientMaxBodySize(ft_stoi(value));
	return true;
}

/*! \brief validates index's value
*       
*  Checks if the index is an existing
*  when index is checkd its value is pushed back to its server.
*/
bool Validator::index( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for index value can not be empty!");
		return false;
	}
	if (!isFile(value)){
		Logger::log(E_ERROR, COLOR_RED, "Index has to be an existing file!");
		return false;
	}
	//push back to its server
	servers[servers.size() - 1].setIndex(value);
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

bool Validator::cgiRoot( std::string value ){

	(void)value;
	return true;
}

bool Validator::cgiIndex( std::string value ){

	(void)value;
	return true;
}

/*! \brief returns the number of elements aka lines in the server block
*       
*
*  counts how many lines there are in this server block up until the
*  next server block or the end. 
*/
size_t Validator::countServerLines(std::vector<std::string>*	lines){

	serverLines = 1;
	while ( (*lines).size() > serverLines && (*lines)[serverLines].compare("server") != 0 ){
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
bool Validator::checkBraces(std::vector<std::string>*	lines){
	size_t i = 1;
	// if ((*lines)[i] == lines->back() || (*lines)[i].compare("{") != 0){
	// 	Logger::log(E_ERROR, COLOR_RED, "Server block should be enclosed in curly braces!");
	// 	return false;
	// }
	int openBraces = 1;
	// if (!lines->empty()){

	// 	for (std::vector<std::string>::iterator it = lines->begin(); it != lines->end(); it++){
	// 		std::cout << *it << std::endl;
	// 	}
	// }
	while (++i < serverLines ){
		if ((*lines)[i].compare("{") == 0){
			openBraces++;
		}
		else if ((*lines)[i].compare("}") == 0){
			openBraces--;
		}
		if (openBraces < 0){
			Logger::log(E_ERROR, COLOR_RED, "Opening curly braces don't match closing ones!");
		return false;
		}
		if (openBraces > 2){
			Logger::log(E_ERROR, COLOR_RED, "There could only be 2 layers of blocks in config file, the server block and the main/location block");
		return false;
		}
		if (openBraces == 0)
			break;
	}
	if (openBraces != 0){
		Logger::log(E_ERROR, COLOR_RED, "Opening curly braces don't match closing ones!");
		return false;
	}
	if (i != serverLines - 1){
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
bool  Validator::storeInnerBlock(std::vector<std::string>*	lines, size_t i){

	//if innerBlock is not empty loop through and earases all 
	while (!innerBlock.empty())
		innerBlock.erase(innerBlock.begin());
		
    std::string key;
    std::string value;
	std::string	root = "";

	while ((*lines)[i] != lines->back() && (*lines)[i].compare("}") != 0 && i != serverLines - 1){
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
				if (key.compare("root") == 0 && values.size() >= 1) 
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
		root.append("/");
		root.append(innerBlock.find("index")->second[0]);
		innerBlock.find("index")->second[0] = root;
	}
	//when innerBlock is saved in inner block map removes the coresponding lines from lines 
	while (i >= 0){
		lines->erase(lines->begin());
		serverLines--;
		if (i == 0)
			break;
		i--;
	}

	return true;
}

bool Validator::checkCgiBlockKeyValues(){
	if (lines[1] == lines.back() || lines[1].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Cgi location block has to be enclosed in curly braces!");
		return false;
	}
	if (!storeInnerBlock(&lines, 1)){
		return false;
	}
	t_location_block_functs  locationFunct[] = { &Validator::allowedMethods, &Validator::cgiRoot, &Validator::cgiIndex, &Validator::autoIndex, &Validator::cgiExt, &Validator::cgiPath};
	//validate key values till the closing }
	return true;
}

bool Validator::checkLocationBlockKeyValues(std::string	locationKey){
	if(locationKey.compare("/cgi-bin") == 0){
		return(checkCgiBlockKeyValues());
	}

	if (locationKey.empty() || locationKey.compare("/")){
		Logger::log(E_ERROR, COLOR_RED, "Cgi location block has to be enclosed in curly braces!");
		return false
	}
	if (lines[1] == lines.back() || lines[1].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Cgi location block has to be enclosed in curly braces!");
		return false;
	}
	if (!storeInnerBlock(&lines, 1)){
		return false;
	}
	t_location_block_functs  locationFunct[] = {&Validator::autoIndex, &Validator::returnKey,
				&Validator::alias, &Validator::clientBodyLimit, &Validator::allowedMethods};
	//validate key values till the closing }
	return true;
}

bool Validator::checkLocationBlock(std::vector<std::string>*	lines){	
	if ((*lines)[0] == lines->back() || (*lines)[2].compare("location /") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Server block has to at least have a location block for root!");
		return false;
	}
	if (!checkLocationBlockKeyValues("/")){
		return false;
	}
	while ((*lines)[0] != lines->back()){
		if((*lines)[2].compare(0, 9, "location ") != 0){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid location block!", (*lines)[2]);
			return false;
		}
		if(!checkLocationBlockKeyValues((*lines)[2].substr ((*lines)[2].find("location ")))){
			return false;
		}
	}
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
				&Validator::clientMaxBodySize, &Validator::index, &Validator::errorPage, &Validator::errorPage, &Validator::errorPage};
	// if (!lines.empty()){

	// 	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++){
	// 		std::cout << *it << std::endl;
	// 	}
	// }
	std::vector<int> keys;
	Server	server;
	servers.push_back(server);
	for (std::map<std::string, std::vector<std::string> >::iterator outerIt = innerBlock.begin(); outerIt != innerBlock.end(); outerIt++){
		//std::cout << "key : " << outerIt->first << std::endl;
		//std::cout << "value : " << outerIt->second[0] << std::endl;
		int i = 0;
		while (i < 9 && valid_main_keys[i].compare(outerIt->first))
			i++ ;
		if (i == 9){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid key.", (*outerIt).first.c_str());
			return false;
		}
		keys.push_back(i);
		if ( outerIt->second.size() > 1 ){
			Logger::log(E_ERROR, COLOR_RED, "%s can not have more than one value.", (outerIt->first).c_str());
			return false;
		}
		//push back one more server to the server vector
		for (std::vector<std::string>::iterator innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
			//std::cout << "value : " << *innerIt << std::endl;
			if (!mainFunct[i](*innerIt)){
				Logger::log(E_ERROR, COLOR_RED, "%s is not a valid value.", (*innerIt).c_str());
				return false;
			}
		}
	}
	for (int i = 0; i < 9; i++){
		if (std::find(keys.begin(), keys.end(), i) == keys.end()){
			Logger::log(E_ERROR, COLOR_RED, "%s is a required key.", valid_main_keys[i].c_str());
			return false;
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
bool Validator::checkMainBlock(std::vector<std::string>*	lines){

	if ((*lines)[2] == lines->back() || (*lines)[2].compare("main") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Server block has to begin with a main block!");
		return false;
	}

	if ((*lines)[3] == lines->back() || (*lines)[3].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Main block should be enclosed in curly braces!");
		return false;
	}
	if (!storeInnerBlock(lines, 4))
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
bool Validator::validate_server(std::vector<std::string>*	lines){

	if (!checkBraces(lines)){
		Logger::log(E_ERROR, COLOR_RED, "Please also note that lines with braces can not be followed by any charachters, even whitespace!");
		return false;
	}
	if (!checkMainBlock(lines))
		return false;
	if (!checkLocationBlock(lines))
		return false;
	else{
		//push back one more server to the server vector
		Server	server;
		servers.push_back(server);
		//push back main block to it
		servers[servers.size() - 1].setLocation(innerBlock, "main");
	}
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
	serverLines = countServerLines(lines);
	if (!validate_server(lines))
		return false;
	// if (!(*lines).empty())
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
	if(infile.fail() == true || infile.bad() == true){
		Logger::log(E_ERROR, COLOR_RED, "Failed to open config file!");
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

std::vector<Server>* Validator::parse(std::string	input){
	if (Validator::validate(input)){
		std::cout << " server 1 index: " << Validator::servers[0].getIndex() << std::endl; 
		return &servers;
	}
	return NULL;
}
//To do:
//check if error pages exist.
//check if files like index.html can be opened.
