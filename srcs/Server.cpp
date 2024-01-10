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
*	@param 
*/
void Server::setKeyValueInLocation(std::string locationBlockKey, std::string key, std::vector<std::string> values) {
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator outerMapIt = this->location_.find(locationBlockKey);

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
        this->location_[locationBlockKey] = newInnerMap;
    }
}

/*! \brief returns the listening port as an int
*       
*  Returns the listening port as an int.
*
*/
void	Server::setLocation( map_of_str_vec_of_str innerBlock, std::string key ) {

	(this->location_)[key] = innerBlock;
}

/*************ssalmi's functions for server management*************/

/*! \brief returns the listening port as an int
*       
*  Returns the listening port as an int.
*
*/
int	Server::setupServer( void ) {

	int	listener_fd;
	int	yes = 1;	// for setsockopt()

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

	setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));	// supresses error message if port is reused

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

/*! \brief sets the upload store directory
*       
*  Creats a directory with the string passed to it,
*  and sets the private upload_store variable.
*
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

/*! \brief returns the listening port as an int
*       
*  Returns the listening port as an int.
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

/*! \brief returns the listening port as an int
*       
*  Returns the listening port as an int.
*
*/
int	Server::getListeningPortInt( void ) const {

	return this->listening_port_;
}

/*! \brief returns the listening port as a string
*       
*  Returns the listening port as a string.
*
*/
std::string	Server::getListeningPortString( void ) const {

	std::ostringstream strStream;

	strStream << this->listening_port_;
	return strStream.str();
}

/*! \brief returns the server name
*       
*  Returns the server name.
*
*/
std::string		Server::getServerName( void ) const {

	return this->server_name_;
}

/*! \brief returns the root
*       
*  Returns the root.
*
*/
std::string	Server::getRoot( void ) const {

	return this->root_;
}

/*! \brief returns the host
*       
*  Returns the host.
*
*/
std::string	Server::getHost( void ) const {

	return this->host_;
}

/*! \brief returns the host as and in_addr_t
*       
*  Returns the host as and in_addr_t.
*
*/
in_addr_t	Server::getHostInAddr_t( void ) const {

	return static_cast<in_addr_t>(ft_stoi(this->host_));
}

/*! \brief returns the lient max body size
*       
*  Returns the lient max body size.
*
*/
double	Server::getClientMaxBodySize( void ) const {

	return this->client_max_body_size_;
}

/*! \brief returns if there is an html page for the given error code
*       
*  Returns if there is an html page for the given error code.
*
*/
bool	Server::isErrorPage( std::string error_code ) const {

	if (this->error_pages.find(error_code) != error_pages.end())
		return true;
	return false;
}

/*! \brief returns upload_store_ variable
*       
*  Returns upload_store_ variable.
*
*/
const std::string&	Server::getUploadStore( void ) const {

	return this->upload_store_;
}

/*! \brief returns the html page for the given error code
*       
*  Returns the html page for the given error code.
*
*/
std::string	Server::getErrorPage( std::string error_code ) const {

	return (this->error_pages.find(error_code)->second);
}

/*! \brief returns the number of location blocks
*       
*  Returns the number of location blocks.
*
*/
int	Server::getLocationBlockCount( void ) const {

	return (this->location_.size());
}

/*! \brief returns a list of location block keys
*       
*  Returns a list of location block keys.
*
*/
std::vector<std::string>	Server::getLocationBlockKeys( void ) const {

	std::vector<std::string> locationBlockKeys;

	for (const_it_for_map_of_str_map_of_str_vec_of_str it = this->location_.begin(); it != this->location_.end(); it++)
		locationBlockKeys.push_back(it->first);
	return locationBlockKeys;
}

/*! \brief returns a list keys within a certain location block
*       
*  Returns a list keys within a certain location block.
*
*/
const std::vector<std::string> Server::getLocationKeys(std::string locationBlockKey) const {

    std::vector<std::string> locationKeys;
    const_it_for_map_of_str_map_of_str_vec_of_str outerMapIt = this->location_.find(locationBlockKey);

    if (outerMapIt != this->location_.end()) {
        const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;

        for (const_it_for_map_of_str_vec_of_str innerMapIt = innerMap.begin(); innerMapIt != innerMap.end(); ++innerMapIt) {
            locationKeys.push_back(innerMapIt->first);
        }
    }
    return locationKeys;
}

/*! \brief returns the number of location block keys
*       
*  Returns the number of location block keys.
*
*/
int	Server::getLocationBlockCount( std::string locationBlockKey ) const {

	int locationKeysCout = 0;
    const_it_for_map_of_str_map_of_str_vec_of_str outerMapIt = this->location_.find(locationBlockKey);

    if (outerMapIt != this->location_.end()) {
        const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;

        for (const_it_for_map_of_str_vec_of_str innerMapIt = innerMap.begin(); innerMapIt != innerMap.end(); ++innerMapIt) {
            locationKeysCout++;
        }
    }
    return locationKeysCout;
}

/*! \brief return the value for a certain key in a certain location
*       
*  Looks for the location in the server with map::find() if the location is found 
*  looks for the key in the location with map::find() and if it is found it's value is returned
*  and else NULL will be returned.
*
*/
const std::vector<std::string>*	Server::getLocationValue( std::string locationBlockKey, std::string key ) const{
	
	const_it_for_map_of_str_map_of_str_vec_of_str outerMapIt = this->location_.find(locationBlockKey);
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
*/
std::string	Server::getCgiExecutor( std::string extension ) const{
	
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
*/
bool Server::isKeyInLocation( std::string locationBlockKey, std::string key ) const {
	if (this->getLocationValue( locationBlockKey, key))
		return true;
	else 
		return false;
}

/*! \brief checks if a certain location exists
*       
*  loops through all the location blocks of the server
*  If the location in question exists returns ture, else it retuns false.
*
*/
bool	Server::isLocationInServer( std::string locationBlockKey ) const {

	for ( const_it_for_map_of_str_map_of_str_vec_of_str it = this->location_.begin(); it != this->location_.end(); it++ ) {
		if (locationBlockKey.compare( it->first ) == 0)
			return true;
	}
	return false;
}

/*! \brief checks if a certain value is listed for a certain key in a certan location
*       
*  checks if a certain value is listed for a certain key in a certan location.
*
*/
bool	Server::isValueListedForKey( std::string locationBlockKey, std::string key, std::string value ) const {

	const std::vector<std::string>*	values = this->getLocationValue(locationBlockKey, key);
		
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
*/
bool	Server::isExtensionOnCgiList( std::string extension ) const {

	return isValueListedForKey("/cgi-bin", "cgi_ext", extension);
}

/*! \brief checks if a certain script is listed under cgi scripts 
*       
*  checks if a certain script is listed under cgi scripts .
*
*/
bool	Server::isScriptOnCgiList( std::string script ) const {

	return isValueListedForKey("/cgi-bin", "index", script);
}
