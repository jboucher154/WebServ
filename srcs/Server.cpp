#include "Server.hpp"

/*! \brief Server class constructor
*  
*	Server class constructor, This constructor is not public and it is advised that
*	it is not used, since it leaves the required fields of a Server uninitialized.
*
*/
Server::Server() {}

/*! \brief Server class copy constructor
*       
* 	Server class copy constructor, calls on assignment operator to asign the instance.
*	
*	@param src server object to copy
*
*/
Server::Server( const Server& src ) {

	*this = src;
}

/*! \brief Server class asignment operator
*       
* 	Server class asignment operator, initializes all existing values with passed values from rhs instance.
*
*	@param rhs server object whose values are copied into the current instance
*	@return @b Server& reference to current instance
*/
Server& Server::operator=( const Server& rhs ) {

	if (this != &rhs ){
		this->listening_port_ = rhs.getListeningPortInt();
		this->server_name_ = rhs.getServerName();
		this->host_ = rhs.getHost();
		this->root_ = rhs.getRoot();
		this->address_ = rhs.getAddress();
		this->client_max_body_size_ = rhs.getClientMaxBodySize();
		this->error_pages = rhs.error_pages;
		this->location_ = rhs.location_;
		this->upload_store_ = rhs.upload_store_;
	}
	return *this;
}

/*! \brief Server class destructor
*       
*  Server class destructor, destroys the instance.
*
*/
Server::~Server() {}

/*! \brief sets a value for the listening port
*       
*  	Sets a value for the listening port.
*	
*	@param port integer to be set as listening port
*/
void	Server::setListeningPort( int port ) {

	this->listening_port_ = port;
}

/*! \brief sets a value for the server name
*       
*  sets a value for the server name.
*
*	@param new_server_name string to set the server name as
*/
void	Server::setServerName( std::string new_server_name ) {

	this->server_name_ = new_server_name;
}

/*! \brief sets a value for the root
*       
*  sets a value for the root.
*
*	@param root string to set the root location of served content
*/
void	Server::setRoot( std::string root ) {

	this->root_ = root;
}

/*! \brief sets a value for the host
*       
*  sets a value for the host.
*
*	@param host string of host ip from config
*/
void	Server::setHost( std::string host ) {

	this->host_ = host;
}

/*! \brief sets a value for the client max body size
*       
*	sets a value for the client max body size.
*
*	@param client_max_body_size double from config main block for max body size from client
*/
void	Server::setClientMaxBodySize( double client_max_body_size ) {

	this->client_max_body_size_ = client_max_body_size;
}

/*! \brief getter for sockaddr_in address info for server object
*       
*	@return @b sockaddr_in returns copy of server address
*/
struct sockaddr_in	Server::getAddress( void ) const {

	return this->address_;
}

/*! \brief sets a value for error page
*       
*	sets a value for error page in the erro_pages map with string of error 
*	code as key and path to custom page as the value.
*
*	@param error_code string representation of an HTPP error code
*	@param error_page string of path to the custom error page provided in config
*/
void	Server::setErrorPage( std::string error_code, std::string error_page ) {

	this->error_pages[error_code] = error_page;
}

/*! \brief add a new element to the location map
*       
*	If the location exist and the key exist in the location block, updates the value'
*	If the location exist and the key doesn't exist in the location block creats the key and value pair
*	If the location block doesn't exist creates it and adds the key and value pair
*
*	@param location_block_key name of the location from the config file
*	@param key	key to add to the location given
*	@param values values to assign to the passed key
*/
void Server::setKeyValueInLocation(std::string location_block_key, std::string key, std::vector<std::string> values) {

    std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator outerMapIt = this->location_.find(location_block_key);

    if (outerMapIt != this->location_.end()) {
        std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;
        std::map<std::string, std::vector<std::string> >::iterator innerMapIt = innerMap.find(key);

        if (innerMapIt != innerMap.end()) {
            innerMapIt->second = values;
        } else {
            innerMap[key] = values;
        }
    } else {
        std::map<std::string, std::vector<std::string> > newInnerMap;
        newInnerMap[key] = values;
        this->location_[location_block_key] = newInnerMap;
    }
}

/*! \brief setter for adding a location with its map of key and values
*       
*	Adds key as location and inner_block as the values in the server objects location map.
*
*	@param inner_block map of key and values for the location
*	@param key name of location to be added
*/
void	Server::setLocation( map_of_str_vec_of_str inner_block, std::string key ) {

	(this->location_)[key] = inner_block;
}

/*************ssalmi's functions for server management*************/

/*! \brief sets up server object socket and address information
*       
*	Server socket and address are intialized in this function. 
*	- TCP socket is opened and set to non-blocking. 
*	- sockaddr_in is populated with server information
*	- setsockopt() is set to supress error messages if a port is reused
*	- the socket and address are bound together with bind()
*	- Lastly, listen() is called on the socket fd to prepare for incoming requests
*	
*	If all function calls are successful, the logger will print out a success message
*
*	@return @b int file descriptor to the socket created for the server object
*/
int	Server::setupServer( void ) {

	int	listener_fd;
	int	yes_for_setsockopt = 1;

	if ((listener_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "server socket error: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		return -1;
	}
	if (fcntl(listener_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "server fcntl error: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		close(listener_fd);
		return -1;
	}
	this->address_.sin_family = AF_INET;
	this->address_.sin_port = htons(this->listening_port_);
	this->address_.sin_addr.s_addr = inet_addr(this->host_.c_str());
	memset(this->address_.sin_zero, '\0', sizeof this->address_.sin_zero);
	setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes_for_setsockopt, sizeof(int));
	if (bind(listener_fd, (struct sockaddr *)&this->address_, sizeof this->address_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "server bind error: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		close(listener_fd);
		return -1;
	}
	if (listen(listener_fd, LISTEN_BACKLOG) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "server listern error: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		close(listener_fd);
		return -1;
	}
	Logger::log(E_INFO, COLOR_WHITE, "%s initialized successfully...", this->getServerIdforLog().c_str());
	return listener_fd;
}

/*! \brief verifies the upload store directory existence and creates it if needed
*       
*	Creats a directory for temporary files using hte server root, name, port and postfixed with the 
*	string passed to it. Sets the private upload_store variable.
*
*	@param upload_dir string to postfix to directory name
*	@return @b bool false if directory could not be made, otherwise true
*/
bool	Server::setUploadStore( std::string upload_dir) {
	std::string upload_store = this->getRoot() + "/." + this->getServerName() + this->getListeningPortString() + upload_dir;
	if (access(upload_store.c_str(), F_OK) == 0){
		Logger::log(E_ERROR, COLOR_RED, "server failed to create temprary upload directory. They already exist. Please remove or rename.");
		return false;
	}
	if (mkdir(upload_store.c_str(), 0777) != 0){
		Logger::log(E_ERROR, COLOR_RED, "server failed to create temprary upload directory");
		return false;
	}
	this->upload_store_ = upload_store;
	return true;

}

/*! \brief getter that formats the servid id information to be printed to the logging
*       
*  @return @b std::string formated id for server object
*
*/
std::string	Server::getServerIdforLog( void ) const {
	
	std::string	id;

	id += "server ";
	id += this->server_name_;
	id += " [HOST: ";
	id += this->host_;
	id += ", PORT: ";
	id += intToString(this->listening_port_);
	id += "]"; 
	return id;
}

/**************************/

/*! \brief getter for server object listening_port_
*       
*	@return @b int of server listening port
*
*/
int	Server::getListeningPortInt( void ) const {

	return this->listening_port_;
}

/*! \brief getter for the listening port as a string
*       
*  @return @b std::string the listening port as a string.
*
*/
std::string	Server::getListeningPortString( void ) const {

	std::ostringstream strStream;

	strStream << this->listening_port_;
	return strStream.str();
}

/*! \brief getter for the server object server name
*       
*  @return @b std::string of the server name
*
*/
std::string		Server::getServerName( void ) const {

	return this->server_name_;
}

/*! \brief getter for the server object root
*       
*  @return @b std::string of the server object root
*
*/
std::string	Server::getRoot( void ) const {

	return this->root_;
}

/*! \brief getter for the server ip from config
*       
*  @return @b std::string of server ip from config file
*
*/
std::string	Server::getHost( void ) const {

	return this->host_;
}

/*! \brief getter for server object in_addr_t
*       
*  @return @b in_addr_t the host as and in_addr_t struct
*
*/
in_addr_t	Server::getHostInAddr_t( void ) const {

	return static_cast<in_addr_t>(ft_stoi(this->host_));
}

/*! \brief gtter for the server object client max body size
*       
*  @return @b double the client max body size allowed
*
*/
double	Server::getClientMaxBodySize( void ) const {

	return this->client_max_body_size_;
}

/*! \brief getter that returns if an error page has been provided for the status code given
*  
*	@param error_code string of numeric HTPP status code representing an error status
*	@return @b bool true  if there is an html page for the given error code, false is none exists.
*
*/
bool	Server::isErrorPage( std::string error_code ) const {

	if (this->error_pages.find(error_code) != error_pages.end())
		return true;
	return false;
}

/*! \brief getter for the server's upload_store_ path for temporary files
*       
*	@return @b std::string& the path to the server object upload_store_
*
*/
const std::string&	Server::getUploadStore( void ) const {

	return this->upload_store_;
}

/*! \brief getter for paths to custom html error pages
*  
*	@param error_code string representaiton of the HTTP status code for an error
*	@return @b std::string the path to the html page for the given error code
*
*/
std::string	Server::getErrorPage( std::string error_code ) const {

	return (this->error_pages.find(error_code)->second);
}

/*! \brief getter for all locations belonging to the server object
*       
*  @return @b std::vector<std::string> with list all existing location keys for the server
*
*/
std::vector<std::string>	Server::getLocationBlockKeys( void ) const {

	std::vector<std::string> locationBlockKeys;

	for (const_it_for_map_of_str_map_of_str_vec_of_str it = this->location_.begin(); it != this->location_.end(); it++)
		locationBlockKeys.push_back(it->first);
	return locationBlockKeys;
}

/*! \brief getter for all keys belonging to one location
*  
*	@param location_block_key name of location to return the keys for
*	@return @b const @b std::vector<std::string> of a list keys within a certain location block.
*
*/
const std::vector<std::string> Server::getLocationKeys(std::string location_block_key) const {

    std::vector<std::string> locationKeys;
    const_it_for_map_of_str_map_of_str_vec_of_str outerMapIt = this->location_.find(location_block_key);

    if (outerMapIt != this->location_.end()) {
        const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;

        for (const_it_for_map_of_str_vec_of_str innerMapIt = innerMap.begin(); innerMapIt != innerMap.end(); ++innerMapIt) {
            locationKeys.push_back(innerMapIt->first);
        }
    }
    return locationKeys;
}

/*! \brief getter for values of specified key in a given location
* 
*  Looks for the location in the server with map::find() if the location is found 
*  looks for the key in the location with map::find() and if it is found it's value is returned
*  and else NULL will be returned.
*
*	@param location_block_key location to seach for key in
*	@param key	key whose values are to be returned
*	@return @b const @b std::vector<std::string>* all values for the given key
*
*/
const std::vector<std::string>*	Server::getLocationValue( std::string location_block_key, std::string key ) const {
	
	const_it_for_map_of_str_map_of_str_vec_of_str outerMapIt = this->location_.find(location_block_key);
	if (outerMapIt != this->location_.end()) {
		const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;
		const_it_for_map_of_str_vec_of_str innerMapIt = innerMap.find(key);
		if (innerMapIt != innerMap.end())
			return &(innerMapIt->second);
		else
			return NULL;
	}
	return NULL;
}

/*! \brief finds the cgi executer's path and returns it
*       
*  Given that the config file requires same indexes for allowed extentions 
*  and their associated executer's path(for example the pyhton3 interpreter)
*  this method loops through the extentions and at the same time increments
*  the cgi path iterator, returning the matched path to the extention.
*
*	@param extension file extension from script in uri
*	@return @b std::string path of executer given in config
*/
std::string	Server::getCgiExecutor( std::string extension ) const {
	
	std::vector<std::string>::const_iterator pathtIt = (getLocationValue("/cgi-bin", "cgi_path"))->begin();
	for (std::vector<std::string>::const_iterator extIt = (getLocationValue("/cgi-bin", "cgi_ext"))->begin(); extIt != (getLocationValue("/cgi-bin", "cgi_ext"))->end(); extIt++) {
		if (*extIt == extension)
			break;
		pathtIt++;
	}
	return *pathtIt;
}

/*! \brief checks if a certain key exists in a certain location
*       
*  calls the getter that would return the value for the key in question
*  If the value is null then either the key didn't exist or the location 
*  If the value is not null the key exists in the location in question.
*
*	@param location_block_key name of location in server object from config
*	@param key to search for in given location
*	@return @b bool true if key exists in location, otherwise false
*/
bool Server::isKeyInLocation( std::string location_block_key, std::string key ) const {

	if (this->getLocationValue( location_block_key, key))
		return true;
	else 
		return false;
}

/*! \brief checks if a certain location exists
*       
*  loops through all the location blocks of the server to find location.
*
*	@param location_block_key name of location to search for in server locations
*	@return @b bool true if the location in question exists, else false.
*/
bool	Server::isLocationInServer( std::string location_block_key ) const {

	for ( const_it_for_map_of_str_map_of_str_vec_of_str it = this->location_.begin(); it != this->location_.end(); it++ ) {
		if (location_block_key.compare( it->first ) == 0)
			return true;
	}
	return false;
}

/*! \brief checks if a certain value is listed for a certain key in a certan location
*       
*	searches for a value and checks if it already exists for the specified key in the given location
*
*	@param location_block_key name of the location key is in
*	@param key	name of the key in the location given
*	@param value to search for in the existing values of the key
*	@return @b bool true if value already exists, otherwise false
*/
bool	Server::isValueListedForKey( std::string location_block_key, std::string key, std::string value ) const {

	const std::vector<std::string>*	values = this->getLocationValue(location_block_key, key);
		
		if (!values || values->empty())
			return false;
		else if (std::find(values->begin(), values->end(), value) == values->end())
			return false;
		else
			return true;
}

/*! \brief checks if a certain extention is listed under cgi extentions 
*       
*  checks if a certain extention is listed under cgi extentions .
*
*	@param extension extension of cgi script requested
*	@result @b bool true if the extension is on the approved list from config
*/
bool	Server::isExtensionOnCgiList( std::string extension ) const {

	return isValueListedForKey("/cgi-bin", "cgi_ext", extension);
}

/*! \brief checks if a certain script is listed under cgi scripts 
*       
*  checks if a certain script is listed under cgi scripts.
*
*	@param script name of script from request
*	@return	@b bool true if script is on list of approved scripts from config
*/
bool	Server::isScriptOnCgiList( std::string script ) const {

	return isValueListedForKey("/cgi-bin", "script", script);
}
