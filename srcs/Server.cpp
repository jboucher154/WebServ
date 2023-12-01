#include "Server.hpp"
#include "Logger.hpp"
#include "Color.hpp"
#include "utility.hpp"

// put these in its own file? Used by Client class as well
#include <fcntl.h>
#include <unistd.h>

// Macro used for listen()
#define	LISTEN_BACKLOG	20

/*! \brief Server class constructor
*       
*
*  Server class constructor, This constructor is not public and it is advised that
*  it is not used, since it leaves the required fields of a Server uninitialized.
*/
Server::Server(){
}

/*! \brief Server class public constructor
*       
*
*  Server class constructor. Constructs an instance and initializes the required values with passed values.
*/
Server::Server( std::string serverName, int port,  std::string host, std::string root
		, std::string index, std::string client_max_body_size ){
	this->setServerName(serverName);
	this->setHost(host);
	this->setRoot(root);
	this->setIndex(index);
	this->setClientMaxBodySize(ft_stoi(client_max_body_size));
	this->setListeningPort(port);
	std::string innerValues[] = {"HEAD", "GET", "POST"};//
	size_t numValues = sizeof(innerValues) / sizeof(innerValues[0]);
	std::vector<std::string> values(innerValues, innerValues + numValues);
	this->setKeyValueInLocation( "/blue", "allow_methods", values );
	this->setKeyValueInLocation( "/", "allow_methods", values );

}

/*! \brief Server class copy constructor
*       
*
*  Server class copy constructor, calls on assignment operator to asign the instance.
*/
Server::Server( const Server& src ){
	*this = src;
}

/*! \brief Server class asignment operator
*       
*
*  Server class asignment operator, initializes all existing values with passed values from rhs instance.
*/
Server& Server::operator=( const Server& rhs ){
	if (this != &rhs ){
		this->listening_port_ = rhs.getListeningPortInt();
		this->server_name_ = rhs.getServerName();
		this->host_ = rhs.getHost();
		this->root_ = rhs.getRoot();
		this->address_ = rhs.getAddress();
		this->client_max_body_size_ = rhs.getClientMaxBodySize();
		this->index_ = rhs.getIndex();
		this->error_page_404 = rhs.getErrorPage_404();
		this->error_page_405 = rhs.getErrorPage_405();
		this->error_page_500 = rhs.getErrorPage_500();
		this->location = rhs.location;
	}
	return *this;
}

/*! \brief Server class destructor
*       
*
*  Server class destructor, destroys the instance.
*/
Server::~Server(){
}

/*! \brief sets a vlaue for the listening port
*       
*
*  sets a vlaue for the listening port.
*/
void	Server::setListeningPort( int port ){

	this->listening_port_ = port;
}

/*! \brief sets a vlaue for the server name
*       
*
*  sets a vlaue for the server name.
*/
void	Server::setServerName( std::string serverName ){

	this->server_name_ = serverName;
}

/*! \brief sets a vlaue for the root
*       
*
*  sets a vlaue for the root.
*/
void	Server::setRoot( std::string root ){
	this->root_ = root;
}

/*! \brief sets a vlaue for the host
*       
*
*  sets a vlaue for the host.
*/
void	Server::setHost( std::string host ){

	this->host_ = host;
}

/*! \brief sets a vlaue for the client max body size
*       
*
*  sets a vlaue for the client max body size.
*/
void	Server::setClientMaxBodySize( int clientMaxBodySize ){

	this->client_max_body_size_ = clientMaxBodySize;
}

/*! \brief sets a vlaue for the index html
*       
*
*  sets a vlaue for the index html.
*/
void	Server::setIndex( std::string index ){

	this->index_ = index;
}

/*! \brief sets a vlaue for error page 404
*       
*
*  sets a vlaue for error page 404.
*/
void	Server::setErrorPage_404( std::string errorPage ){

	this->error_page_404 = errorPage;
}

/*! \brief sets a vlaue for error page 405
*       
*
*  sets a vlaue for error page 405.
*/
void	Server::setErrorPage_405( std::string errorPage ){

	this->error_page_405 = errorPage;
}

/*! \brief sets a vlaue for error page 500
*       
*
*  sets a vlaue for error page 500.
*/
void	Server::setErrorPage_500( std::string errorPage ){

	this->error_page_500 = errorPage;
}

/*! \brief add a new element to the location map
*       
*
*  If the location exist and the key exist in the location block, updates the value'
*  If the location exist and the key doesn't exist in the location block creats the key and value pair
*  If the location block doesn't exist creats it and adds the key and value pair
*/

void Server::setKeyValueInLocation(std::string locationBlockKey, std::string key, std::vector<std::string> values) {
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator outerMapIt = this->location.find(locationBlockKey);

    if (outerMapIt != this->location.end()) {
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
        this->location[locationBlockKey] = newInnerMap;
    }
}

void	Server::setLocation( std::map<std::string, std::vector<std::string> >	innerBlock, std::string key ){

	(this->location)[key] = innerBlock;
}

/*************ssalmi's functions for server management*************/

int	Server::setupServer( void ) {

	int	listener_fd;
	int	yes = 1;	// for setsockopt()

	if ((listener_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "server socket error: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		return -1;
	}

	if (fcntl(listener_fd, F_SETFL, O_NONBLOCK) == -1) {
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
*
*  Returns the listening port as an int.
*/
int	Server::getListeningPortInt( void ) const{

	return (this->listening_port_);
}

/*! \brief returns the listening port as a string
*       
*
*  Returns the listening port as a string.
*/
std::string	Server::getListeningPortString( void ) const{

	std::ostringstream strStream;

	strStream << this->listening_port_;
	return (strStream.str());
}

/*! \brief returns the server name
*       
*
*  Returns the server name.
*/
std::string		Server::getServerName( void ) const{

	return (this->server_name_);
}

/*! \brief returns the root
*       
*
*  Returns the root.
*/
std::string	Server::getRoot( void ) const{

	return (this->root_);
}

/*! \brief returns the host
*       
*
*  Returns the host.
*/
std::string	Server::getHost( void ) const{

	return (this->host_);
}

/*! \brief returns the host as and in_addr_t
*       
*
*  Returns the host as and in_addr_t.
*/
in_addr_t	Server::getHostInAddr_t( void ) const{

	return (static_cast<in_addr_t>(ft_stoi(this->host_)));
}

/*! \brief returns the lient max body size
*       
*
*  Returns the lient max body size.
*/
int	Server::getClientMaxBodySize( void ) const{

	return (this->client_max_body_size_);
}

/*! \brief returns the index html
*       
*
*  Returns the index html.
*/
std::string	Server::getIndex( void ) const{

	return (this->index_);
}

/*! \brief returns the error 404 html
*       
*
*  Returns the error 404 html.
*/
std::string	Server::getErrorPage_404( void ) const{

	return (this->error_page_404);
}

/*! \brief returns the error 405 html
*       
*
*  Returns the error 405 html.
*/
std::string	Server::getErrorPage_405( void ) const{

	return (this->error_page_405);
}

/*! \brief returns the error 500 html
*       
*
*  Returns the error 500 html.
*/
std::string	Server::getErrorPage_500( void ) const{

	return (this->error_page_500);
}


struct sockaddr_in	Server::getAddress( void ) const{

	return(this->address_);
}

/*! \brief returns the number of location blocks
*       
*
*  Returns the number of location blocks.
*/
int	Server::getLocationBlockCount( void ) const{

	return (this->location.size());
}

/*! \brief returns a list of location block keys
*       
*
*  Returns a list of location block keys.
*/
std::vector<std::string>	Server::getLocationBlockKeys( void ) const{

	std::vector<std::string> locationBlockKeys;

	for ( std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = this->location.begin(); it != this->location.end(); it++ )
		locationBlockKeys.push_back(it->first);
	return locationBlockKeys;
}

/*! \brief returns a list keys within a certain location block
*       
*
*  Returns a list keys within a certain location block.
*/
const std::vector<std::string> Server::getLocationKeys(std::string locationBlockKey) const {

    std::vector<std::string> locationKeys;

    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator outerMapIt = this->location.find(locationBlockKey);

    if (outerMapIt != this->location.end()) {
        const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;

        for (std::map<std::string, std::vector<std::string> >::const_iterator innerMapIt = innerMap.begin(); innerMapIt != innerMap.end(); ++innerMapIt) {
            locationKeys.push_back(innerMapIt->first);
        }
    }
    return locationKeys;
}

/*! \brief returns the number of location block keys
*       
*
*  Returns the number of location block keys.
*/
int	Server::getLocationBlockCount( std::string locationBlockKey ) const{

	int locationKeysCout = 0;

    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator outerMapIt = this->location.find(locationBlockKey);

    if (outerMapIt != this->location.end()) {
        const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;

        for (std::map<std::string, std::vector<std::string> >::const_iterator innerMapIt = innerMap.begin(); innerMapIt != innerMap.end(); ++innerMapIt) {
            locationKeysCout++;
        }
    }
    return locationKeysCout;
}

/*! \brief return the value for a certain key in a certain location
*       
*
*  Looks for the location in the server with map::find() if the location is found 
*  looks for the key in the location with map::find() and if it is found it's value is returned
*  and else NULL will be returned.
*/
const std::vector<std::string>*	Server::getLocationValue( std::string locationBlockKey, std::string key ) const{
	
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator outerMapIt = this->location.find(locationBlockKey);
	if (outerMapIt != this->location.end()) {
		const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;
		std::map<std::string, std::vector<std::string> >::const_iterator innerMapIt = innerMap.find(key);
		if ( innerMapIt != innerMap.end() )
			return (&(innerMapIt->second));
		else
			return NULL;
	}
	return NULL;
}

/*! \brief finds the cgi executer's path and returns it
*       
*
*  Given that the config file requires same indexes for allowed extentions 
*  and their associated executer's path(for example the pyhton3 interpreter)
*  this method loops through the extentions and at the same time increments
*  the cgi path iterator, returning the matched path to the extention.
*/
std::string	Server::getCgiExecutor( std::string extention ) const{
	
	
	std::vector<std::string>::const_iterator pathtIt = (getLocationValue("/cgi-bin", "cgi_path"))->begin();
	for (std::vector<std::string>::const_iterator extIt = (getLocationValue("/cgi-bin", "cgi_ext"))->begin(); extIt != (getLocationValue("/cgi-bin", "cgi_ext"))->end(); extIt++){
		if (*extIt == extention)
			break;
		pathtIt++;
	}
	return (*pathtIt);
}

/*! \brief checks if a certain key exists in a certain location
*       
*
*  calls the getter that would return the value for the key in question
*  If the value is null then either the key didn't exist or the location 
*  If the value is not null the key exists in the location in question.
*/
bool Server::isKeyInLocation( std::string locationBlockKey, std::string key ) const{
	if ( this->getLocationValue( locationBlockKey, key) )
		return true;
	else return false;
}

/*! \brief checks if a certain location exists
*       
*
*  loops through all the location blocks of the server
*  If the location in question exists returns ture, else it retuns false.
*/
bool	Server::isLocationInServer( std::string locationBlockKey ) const{

	for ( std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = this->location.begin(); it != this->location.end(); it++ ) {
		if ( locationBlockKey.compare( it->first ) == 0 )
			return true;
	}
	return false;
}

/*! \brief checks if a certain value is listed for a certain key in a certan location
*       
*
*  checks if a certain value is listed for a certain key in a certan location.
*/
bool	Server::isValueListedForKey( std::string locationBlockKey, std::string key, std::string value ) const{

	const std::vector<std::string>*	values = this->getLocationValue(locationBlockKey, key);
		
		if (!values || values->empty())
			return (false);
		else if (std::find(values->begin(), values->end(), value) == values->end())
			return (false);
		else
			return (true);
}

/*! \brief checks if a certain extention is listed under cgi extentions 
*       
*
*  checks if a certain extention is listed under cgi extentions .
*/
bool	Server::isExtentionOnCgiList( std::string extention ) const{

	return (isValueListedForKey("/cgi-bin", "cgi_ext", extention));
}

/*! \brief checks if a certain script is listed under cgi scripts 
*       
*
*  checks if a certain script is listed under cgi scripts .
*/
bool	Server::isScriptOnCgiList( std::string script ) const{

	return (isValueListedForKey("/cgi-bin", "index", script));
}

//to Do
// a getter that takes an exten returns a cgi path
// is there error page(int error cod)
//change getters for error pages so they take int error code and return a path