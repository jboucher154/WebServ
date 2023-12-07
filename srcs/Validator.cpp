#include "Validator.hpp"
std::string valid_main_keys_array[] = {"listen", "server_name", "host", "root",
		"client_max_body_size", "index", "error_page_400", "error_page_401", "error_page_402",
		"error_page_403", "error_page_404", "error_page_405", "error_page_406", "error_page_407",
		"error_page_408", "error_page_409", "error_page_410", "error_page_411", "error_page_412",
		"error_page_413", "error_page_414", "error_page_415", "error_page_416", "error_page_417",
		"error_page_418", "error_page_421", "error_page_422", "error_page_423", "error_page_424",
		"error_page_425", "error_page_426", "error_page_428", "error_page_429", "error_page_431",
		"error_page_451", "error_page_500", "error_page_501", "error_page_502", "error_page_503",
		"error_page_504", "error_page_505", "error_page_506", "error_page_507", "error_page_508",
		"error_page_510", "error_page_511"};
std::vector<std::string> valid_main_keys(valid_main_keys_array, valid_main_keys_array
	+ sizeof(valid_main_keys_array) / sizeof(valid_main_keys_array[0]));
std::string valid_location_keys_array[] = {"autoindex", "return",
		 "alias", "client_body_size", "allow_methods", "root", "index", "cgi_ext", "cgi_path"};
std::vector<std::string> valid_location_keys(valid_location_keys_array, valid_location_keys_array
	+ sizeof(valid_location_keys_array) / sizeof(valid_location_keys_array[0]));

std::vector<std::string>							Validator::lines;
std::map<std::string, std::vector<std::string> >	Validator::innerBlock;
std::vector<Server>									Validator::servers;
size_t 												Validator::serverLines = 0;
std::map<std::string, std::string> 					Validator::validIpHostMap;
std::string											Validator::rootPath = "";
std::string											Validator::mainRootPath = "";

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

/*! \brief Reterieves host name and ip addresses from the system
*       
*  Reads the /etc/hosts file into a line and splits the line
*  into keys and values that are saved in the validIpHostMap.
*  Then gets host name and it ip from the system and adds the 
*  to the map.
*/
bool Validator::validIpHostBuilder(){
	std::string key;
	std::string value;
    // Read the /etc/hosts file
    std::ifstream hostsFile("/etc/hosts");
    if (hostsFile.is_open()) {
        std::string line;
        while (std::getline(hostsFile, line)) {
			if (line[0] == '#')
				continue;
			std::stringstream ss(line);
			if (std::getline(ss, key, ' ') && (ss >> value))
				validIpHostMap[key] = value;
        }
        hostsFile.close();
    }
	else{
        Logger::log(E_ERROR, COLOR_RED,"Error opening /etc/hosts file.");
		return false;
	}
    // Get the host name
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0){
        Logger::log(E_ERROR, COLOR_RED, "Error getting host name.");
		return false;
	}
    // Structure to store address information
    struct addrinfo hints, *result, *rp;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use a stream socket
    // Get address information
    if (getaddrinfo(hostname, nullptr, &hints, &result) != 0) {
        Logger::log(E_ERROR, COLOR_RED, "Error getting address information for %s", hostname);
        return false;
    }
	// Iterate through the results and build the IP address
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
		void* addr;
		char ipstr[INET6_ADDRSTRLEN];

		if (rp->ai_family == AF_INET) { // IPv4
			struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(rp->ai_addr);
			addr = &(ipv4->sin_addr);
		} else { // IPv6
			struct sockaddr_in6* ipv6 = reinterpret_cast<struct sockaddr_in6*>(rp->ai_addr);
			addr = &(ipv6->sin6_addr);
		}
		// Convert the IP address to a readable format
		inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr));
		validIpHostMap[hostname] = std::string(ipstr);
	}
    // Free allocated memory
    freeaddrinfo(result);
    return true;
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
*  the hostname asociated with the already validated host ip.
*  when server name is checkd its value is pushed back to its server.
*/
bool Validator::serverName( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for server name value can not be empty!");
		return false;
	}
	if (servers[servers.size() - 1].getHost() == ""){
		Logger::log(E_ERROR, COLOR_RED, "The field for host value can not be empty!");
		return false;
	}
	if (validIpHostMap.find(servers[servers.size() - 1].getHost())->second != value){
		Logger::log(E_ERROR, COLOR_RED, "The host you provided: %s does not match your servername: %s!",servers[servers.size() - 1].getHost().c_str(), value.c_str());
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
	if (validIpHostMap.find(value) == validIpHostMap.end()){
		Logger::log(E_ERROR, COLOR_RED, "Host is not a valid ip address for this system!");
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
*  Builds up the full path by adding root to index.     
*  Checks if the index is an existing
*  Opens the file and checks if openning permission is there.
*  when index is checkd its value is pushed back to its server.
*/
bool Validator::index( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for index value can not be empty!");
		return false;
	}
	//adds root directory to the begining of the index file
	std::string	temp = rootPath;
	temp.append("/");
	temp.append(value);
	innerBlock.find("index")->second[0] = temp;

	if (!isFile(temp)){
		Logger::log(E_ERROR, COLOR_RED, "Index has to be an existing file!");
		return false;
	}
	if (!canOpen(temp) ){
		Logger::log(E_ERROR, COLOR_RED, "index has to be a file with opening permission!");
		return false;
	}
	//push back to its server
	servers[servers.size() - 1].setIndex(temp);
	return true;
}

/*! \brief validates indexes for error pages
*  
*  Builds up the full path by adding root to index.     
*  Checks if the index for a specific error pages exists
*  Opens the file and checks if openning permission is there.
*  when index is checkd its value is pushed back to its server.
*/
bool Validator::errorPage( std::string value, std::string key ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for index value can not be empty!");
		return false;
	}
	//adds root directory to the begining of the index file
	std::string	temp = rootPath;
	temp.append("/");
	temp.append(value);
	innerBlock.find(key)->second[0] = temp;
	if (!isFile(temp)){
		Logger::log(E_ERROR, COLOR_RED, "%s has to be an existing file!", key.c_str());
		return false;
	}
	if (!canOpen(temp) ){
		Logger::log(E_ERROR, COLOR_RED, "%s has to be a file with opening permission!", key.c_str());
		return false;
	}
	//push back to its server
	servers[servers.size() - 1].setErrorPage(key, value);
	return true;
}

/*! \brief validates allowed methods for location blocks
*  
*  Checks against the list of valid methods and returns false
*  if the value passed is not on the list, and true if it is. 
*  
*/
bool Validator::allowedMethods( std::string value ){

	if( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for allowed methods value can not be empty!");
		return false;
	}
	if(value.compare("GET") == 0 || value.compare("POST") == 0 ||
		value.compare("HEAD") == 0 || value.compare("DELETE") == 0)
		return true;
	else
		return false;
}

/*! \brief validates autoindex for location blocks
*  
*  Autoindex can only have 2 valid values, on and off.
*  Returns true if value is either one of these and false otherwise. 
*  
*/
bool Validator::autoIndex( std::string value ){

	if( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for autoindex value can not be empty!");
		return false;
	}
	if(value.compare("on") == 0 || value.compare("off") == 0)
		return true;
	else
		return false;
}

/*! \brief validates return key's value for location blocks
*  
*  Cofig file for this server has to be written in order meaning
*  that if a location block returns another location block the
*  returned block has to have been written above the block with 
*  the return key. So this method searches for the block that is
*  to be returned and if it is found returns true and false otherwise.
*  
*/
bool Validator::returnKey( std::string value ){

	if( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for return value can not be empty!");
		return false;
	}
	if(servers[servers.size() - 1].isLocationInServer(value))
		return true;
	else
		return false;
}

/*! \brief validates allowed methods for location blocks
*  
*  Cofig file for this server has to be written in order meaning
*  that if a location block aliases another location block the
*  aliased block has to have been written above the block with 
*  the alias key. So this method searches for the block that is
*  to be aliased and if it is found returns true and false otherwise.
*  
*/
bool Validator::alias( std::string value ){

	if( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for alias value can not be empty!");
		return false;
	}
	if(servers[servers.size() - 1].isLocationInServer(value))
		return true;
	else
		return false;
}

/*! \brief validates autoindex for location blocks
*  
*  This server has implications to execute .py and .sh
*  extentions so these are the only valid values for now
*  if the value is either one of these this method returns
*  true, and otherwise it returns false. 
*  
*/
bool Validator::cgiExt( std::string value ){

	if( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for cgi extentions value can not be empty!");
		return false;
	}
	if(value.compare(".sh") == 0 || value.compare(".py") == 0)
		return true;
	else
		return false;
}

/*! \brief validates autoindex for location blocks
*  
*  The path to .sh executor or python interpreter
*  has to be accessable and executable so this methods
*  checkes for accessability and returns true or false
*  accordingly.
*  
*/
bool Validator::cgiPath( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for root value can not be empty!");
		return false;
	}
	if (access (value.c_str(), X_OK) != 0){
		Logger::log(E_ERROR, COLOR_RED, "cgi path has to be an existing directory!");
		return false;	
	}

	return true;
}

/*! \brief validates autoindex for location blocks
*  
*  If a location block overwrites the main root the
*  replacement has to be a directory, so this method
*  checks for that.
*  
*/
bool Validator::locationRoot( std::string value ){

	if ( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for location root value can not be empty!");
		return false;
	}
	if (!isDirectory(value)){
		Logger::log(E_ERROR, COLOR_RED, "location root has to be an existing directory!");
		return false;
	}
	return true;
}

/*! \brief validates autoindex for location blocks
*  
*  This method first updates the index value, attaching
*  the full path to it and then checks for existance of
*  thr index file and opening access right.
*  
*/
bool Validator::locationIndex( std::string value ){

	if( value.empty() ){
		Logger::log(E_ERROR, COLOR_RED, "The field for location index value can not be empty!");
		return false;
	}
	std::string	temp = rootPath;
	temp.append("/");
	temp.append(value);
	std::map<std::string, std::vector<std::string> >::iterator outerIt = innerBlock.find("index");
    // Find the member in the vector
    std::vector<std::string>::iterator vecIt = std::find(outerIt->second.begin(), outerIt->second.end(), value);
    *vecIt = temp;

	if (!isFile(temp)){
		Logger::log(E_ERROR, COLOR_RED, "Index has to be an existing file!");
		return false;
	}
	if (!canOpen(temp) ){
		Logger::log(E_ERROR, COLOR_RED, "index has to be a file with opening permission!");
		return false;
	}
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
	int openBraces = 1;
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
	std::string	temp = "";

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
				if (key.compare("root") == 0 && values.size() >= 1){
					rootPath = values[0];
				} 
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

/*! \brief validates cgi location block key and values
*  
*  The first line of cgi block has to be an opening curly
*  braces. If so storeInner will be called, and if it returns
*  true it means that the closing curly braces is found and the
*  format of key and value being seperated by a space is maintained.
*  from there we check that storeInnerBlock actually created a non_empty
*  block and call the coresponding function to validate each key's value.
*  One last check is to see if that the mandatory keys are validated,
*  and that this is the only cgi block in this server. If all checks
*  are passed cgi block is added to the server.
*/
bool Validator::checkCgiBlockKeyValues(){
	if (lines[1] == lines.back() || lines[1].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Cgi location block has to be enclosed in curly braces!");
		return false;
	}
	if (!storeInnerBlock(&lines, 2)){
		return false;
	}
	if (innerBlock.empty()) { //?
		Logger::log(E_ERROR, COLOR_RED, "Cgi location block can not be empty!");
		return false;
	}
	t_location_block_functs  locationFunct[] = { &Validator::allowedMethods, &Validator::locationRoot, &Validator::locationIndex, &Validator::cgiExt, &Validator::cgiPath};
	//validate key values till the closing }
	std::vector<int> keys;
	for (std::map<std::string, std::vector<std::string> >::iterator outerIt = innerBlock.begin(); outerIt != innerBlock.end(); outerIt++){
		int i = 0;
		while (i < 5 && valid_location_keys[i + 4].compare(outerIt->first))
			i++ ;
		if (i == 5){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid key for Cgi location.", (*outerIt).first.c_str());
			return false;
		}
		keys.push_back(i);
		if ( outerIt->first == "root" && outerIt->second.size() > 1 ){
			Logger::log(E_ERROR, COLOR_RED, "%s can not have more than one value.", (outerIt->first).c_str());
			return false;
		}
		for (std::vector<std::string>::iterator innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
			if (!locationFunct[i](*innerIt)){
				Logger::log(E_ERROR, COLOR_RED, "%s is not a valid value.", (*innerIt).c_str());
				return false;
			}
		}
	}
	for (int i = 0; i < 5; i++){
		if (std::find(keys.begin(), keys.end(), i) == keys.end()){
			Logger::log(E_ERROR, COLOR_RED, "%s is a required key.", valid_main_keys[i].c_str());
			return false;
		}

	}
	//check for no deplication
	//add to server vector
	if ( servers[servers.size() - 1].isLocationInServer("/cgi-bin")){
		Logger::log(E_ERROR, COLOR_RED, "Ambiguity warning: Cgi block dublication is not allowed in the server!");
		return false;
	}
	else{
		servers[servers.size() - 1].setLocation(innerBlock, "/cgi-bin");
	}
	return true;
}

/*! \brief validates cgi location block key and values
*  
*  cgi block will be validated by its own validator.
*  location specific key can not contain spaces or #
*  -that is coment indicator.it has to begin with an opening curly
*  braces. If so storeInner will be called, and if it returns
*  true it means that the closing curly braces is found and the
*  format of key and value being seperated by a space is maintained.
*  from there we check that storeInnerBlock actually created a non_empty
*  block. If this block doesn't have a return or and alias key
*  the default value for its root and index have to be added, and verified
*  default root is the location specific key, and default index is index.html.
*  Now we call the coresponding function to validate each key's value.
*  One last check is to see if that this is the only location block with this
*  location specift key in this server. If all checks
*  are passed cgi block is added to the server.
*/
bool Validator::checkLocationBlockKeyValues(std::string	locationKey){

	if(locationKey.compare("/cgi-bin") == 0){
		return(checkCgiBlockKeyValues());
	}

	if (locationKey.empty() || locationKey.find_first_of(" #") != std::string::npos){
		Logger::log(E_ERROR, COLOR_RED, "here %s is not a valid location block!", locationKey.c_str());
		return false;
	}
	if (lines[1] == lines.back() || lines[1].compare("{") != 0){
		Logger::log(E_ERROR, COLOR_RED, "%s Location block has to be enclosed in curly braces!", locationKey.c_str());
		return false;
	}
	if (!storeInnerBlock(&lines, 2)){
		return false;
	}
	if (innerBlock.empty()) { //?
		Logger::log(E_ERROR, COLOR_RED, "%s location block can not be empty!", locationKey.c_str());
		return false;
	}
	if (innerBlock.find("return") == innerBlock.end() && innerBlock.find("alias") == innerBlock.end()){
		//add default root value if no root value is specified
		if (innerBlock.find("root") == innerBlock.end()){
			//add the location root to main root for compelete path
			std::string	temp = mainRootPath;
			temp.append(locationKey);
			rootPath = temp;
			std::vector<std::string> rootValue;
			rootValue.push_back(rootPath);
			innerBlock["root"] = rootValue;
		}
		//add index default value if no index is specified
		if (innerBlock.find("index") == innerBlock.end()){
			std::vector<std::string> indexValue;
			indexValue.push_back("index.html");
			innerBlock["index"] = indexValue;
		}
	}
	t_location_block_functs  locationFunct[] = {&Validator::autoIndex, &Validator::returnKey,
				&Validator::alias, &Validator::clientMaxBodySize, &Validator::allowedMethods,
				&Validator::locationRoot, &Validator::locationIndex};
	//validate key values till the closing }
	for (std::map<std::string, std::vector<std::string> >::iterator outerIt = innerBlock.begin(); outerIt != innerBlock.end(); outerIt++){
		int i = 0;
		while (i < 7 && valid_location_keys[i].compare(outerIt->first))
			i++ ;
		if (i == 7){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid key for %s location.", (*outerIt).first.c_str(), locationKey.c_str());
			return false;
		}
		if ( outerIt->first == "root" && outerIt->second.size() > 1 ){
			Logger::log(E_ERROR, COLOR_RED, "%s can not have more than one value.", (outerIt->first).c_str());
			return false;
		}
		for (std::vector<std::string>::iterator innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
			if (!locationFunct[i](*innerIt)){
				Logger::log(E_ERROR, COLOR_RED, "%s is not a valid value.", (*innerIt).c_str());
				return false;
			}
		}
	}
	//check for no deplication
	//add to server vector
	if ( servers[servers.size() - 1].isLocationInServer(locationKey)){
		Logger::log(E_ERROR, COLOR_RED, "Ambiguity warning: %s location block dublication is not allowed in the server!", locationKey.c_str());
		return false;
	}
	else
		servers[servers.size() - 1].setLocation(innerBlock, locationKey);
	return true;
}

/*! \brief validates location blocks
*  
*  First line in a location block has to say location /
*  plus the location specific key that will be validated
*  by checkLocationBlockKeyValues. First location block
*  has to be the root, after that as long as there are locations
*  in this server they have to be validated by checkLocationBlockKeyValues.
*/
bool Validator::checkLocationBlock(std::vector<std::string>*	lines){	
	if ((*lines)[0] == lines->back() || (*lines)[0].compare(0, 10,"location /") != 0){
		Logger::log(E_ERROR, COLOR_RED, "Server block has to at least have a location block for root!");
		return false;
	}
	if (!checkLocationBlockKeyValues("/")){
		return false;
	}
	while ((*lines)[0] != lines->back()){
		if((*lines)[0].compare(0, 10, "location /") != 0){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid location block!", (*lines)[0].c_str());
			return false;
		}
		if(!checkLocationBlockKeyValues((*lines)[0].substr(9))){
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
				&Validator::clientMaxBodySize, &Validator::index};
	std::vector<int> keys;
	Server	server;
	servers.push_back(server);
	for (std::map<std::string, std::vector<std::string> >::iterator outerIt = innerBlock.begin(); outerIt != innerBlock.end(); outerIt++){
		int i = 0;
		while (i < 46 && valid_main_keys[i].compare(outerIt->first))
			i++ ;
		if (i == 46){
			Logger::log(E_ERROR, COLOR_RED, "%s is not a valid key.", (*outerIt).first.c_str());
			return false;
		}
		keys.push_back(i);
		if ( outerIt->second.size() > 1 ){
			Logger::log(E_ERROR, COLOR_RED, "%s can not have more than one value.", (outerIt->first).c_str());
			return false;
		}
		if (i >= 6)
			i = 6;
		for (std::vector<std::string>::iterator innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
			if ((i < 6 && !mainFunct[i](*innerIt)) || (i == 6 && !errorPage(*innerIt, outerIt->first))){
				Logger::log(E_ERROR, COLOR_RED, "%s is not a valid value.", (*innerIt).c_str());
				return false;
			}
		}
	}
	for (int i = 0; i < 6; i++){
		if (std::find(keys.begin(), keys.end(), i) == keys.end()){
			Logger::log(E_ERROR, COLOR_RED, "%s is a required key.", valid_main_keys[i].c_str());
			return false;
		}

	}
	mainRootPath = rootPath;
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
	lines->erase(lines->begin());
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
	if (!(*lines).empty())
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
		if(line[0] == '#')
			continue;
		if (line.find_first_of("#") != std::string::npos){
			size_t endPos =line.find_first_of("#");
			line = line.substr(0, endPos);
		}
		lines.push_back(line);
		std::string temp;
		while (ss >> temp) {
			if (!ss.eof()){
			}
		}
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

	if (!validIpHostBuilder()){
		Logger::log(E_ERROR, COLOR_RED, "Server failed to read host name and/or ip addresses from the system!");
		return false;
	}
	if ( !store_lines(input) || lines.empty()){
		Logger::log(E_ERROR, COLOR_RED, "The config file is empty!");
		return false;
	}
	return (validate_lines(&lines));
}
