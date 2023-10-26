#include "Server.hpp"

Server::Server(){
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

void Server::setLocation(std::string locationBlockKey, std::string key, std::vector<std::string> values) {
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator outerMapIt = this->location.find(locationBlockKey);

    if (outerMapIt != this->location.end()) {
        std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;
        std::map<std::string, std::vector<std::string> >::iterator innerMapIt = innerMap.find(key);

        if (innerMapIt != innerMap.end()) {
            innerMapIt->second = values;
        }
    }
}


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
	return (locationBlockKeys);
}


std::vector<std::string> Server::getLocationKeys(std::string locationBlockKey) const {

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

std::vector<std::string>	Server::getLocationValue( std::string locationBlockKey, std::string key ) const{

	std::vector<std::string> value;

	std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator outerMapIt = this->location.find(locationBlockKey);
	if (outerMapIt != this->location.end()) {
        const std::map<std::string, std::vector<std::string> >& innerMap = outerMapIt->second;
		std::map<std::string, std::vector<std::string> >::const_iterator innerMapIt = innerMap.find(key);
		if ( innerMapIt != innerMap.end() )
			value = innerMapIt->second;
	}
	return value;
}
