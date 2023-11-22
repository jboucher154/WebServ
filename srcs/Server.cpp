#include "Server.hpp"
#include "Logger.hpp"
#include "Color.hpp"
#include "utility.hpp"

// put these in its own file? Used by Client class as well
#include <fcntl.h>
#include <unistd.h>

// Macro used for listen()
#define	LISTEN_BACKLOG	20

Server::Server(){
}

Server::Server( std::string serverName, int port,  std::string host ){
	this->setServerName(serverName);
	this->setHost(host);
	this->setRoot("./resources");
	this->setIndex("index.html");
	this->setClientMaxBodySize(30000);
	this->setListeningPort(port);
	std::string innerValues[] = {"HEAD", "GET"};
	size_t numValues = sizeof(innerValues) / sizeof(innerValues[0]);
	std::vector<std::string> values(innerValues, innerValues + numValues);
	this->setLocation( "/blue", "allow_methods", values );
	this->setLocation( "/", "allow_methods", values );

}

Server::Server( const Server& src ){
	*this = src;
}

Server& Server::operator=( const Server& rhs ){
	if (this != &rhs ){
		this->listening_port_ = rhs.getListeningPortInt();
		this->server_name_ = rhs.getServerName();
		this->host_ = rhs.getHost();
		this->root_ = rhs.getRoot();
		this->address_ = rhs.getAddress();
		this->client_max_body_size_ = rhs.getClientMaxBodySize();
		this->index_ = rhs.getIndex();
		this->error_page_ = rhs.getErrorPage();
		this->location = rhs.location;
	}
	return *this;
}

Server::~Server(){
}

void	Server::setListeningPort( int port ){

	this->listening_port_ = port;
}

void	Server::setServerName( std::string serverName ){

	this->server_name_ = serverName;
}

void	Server::setRoot( std::string root ){
	this->root_ = root;
}

void	Server::setHost( std::string host ){

	this->host_ = host;
}

void	Server::setClientMaxBodySize( int clientMaxBodySize ){

	this->client_max_body_size_ = clientMaxBodySize;
}

void	Server::setIndex( std::string index ){

	this->index_ = index;
}

void	Server::setErrorPage( std::string errorPage ){

	this->error_page_ = errorPage;
}

/*! \brief add a new element to the location map
*       
*
*  If the location exist and the key exist in the location block, updates the value'
*  If the location exist and the key doesn't exist in the location block creats the key and value pair
*  If the location block doesn't exist creats it and adds the key and value pair
*/

void Server::setLocation(std::string locationBlockKey, std::string key, std::vector<std::string> values) {
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


/*************ssalmi's functions for server management*************/

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

int	Server::getListeningPortInt( void ) const{

	return (this->listening_port_);
}

std::string	Server::getListeningPortString( void ) const{

	std::ostringstream strStream;

	strStream << this->listening_port_;
	return (strStream.str());
}

std::string		Server::getServerName( void ) const{

	return (this->server_name_);
}

std::string	Server::getRoot( void ) const{

	return (this->root_);
}

std::string	Server::getHost( void ) const{

	return (this->host_);
}

in_addr_t	Server::getHostInAddr_t( void ) const{

	return (static_cast<in_addr_t>(ft_stoi(this->host_)));
}

int	Server::getClientMaxBodySize( void ) const{

	return (this->client_max_body_size_);
}

std::string	Server::getIndex( void ) const{

	return (this->index_);
}

std::string	Server::getErrorPage( void ) const{

	return (this->error_page_);
}

struct sockaddr_in	Server::getAddress( void ) const{

	return(this->address_);
}

int	Server::getLocationBlockCount( void ) const{

	return (this->location.size());
}

std::vector<std::string>	Server::getLocationBlockKeys( void ) const{

	std::vector<std::string> locationBlockKeys;

	for ( std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = this->location.begin(); it != this->location.end(); it++ )
		locationBlockKeys.push_back(it->first);
	return locationBlockKeys;
}


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

bool Server::isKeyInLocation( std::string locationBlockKey, std::string key ) const{
	if ( this->getLocationValue( locationBlockKey, key) )
		return true;
	else return false;
}

bool	Server::isLocationInServer( std::string locationBlockKey ) const{

	// if (this->location.empty())
	// 	return false;
	for ( std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = this->location.begin(); it != this->location.end(); it++ ) {
		if ( locationBlockKey.compare( it->first ) == 0 )
			return true;
	}
	return false;
}