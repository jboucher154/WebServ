#include "Client.hpp"

// remove later?
#include <cstring>

/* CONSTRUCTORS */

Client::Client( void ) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	this->latest_time_ = time(0);

	this->resetRequest();
	this->resetResponse();

	this->server_ = NULL;
	this->server_fd_ = -1;
}

Client::Client( int server_fd, Server* server ) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	this->latest_time_ = time(0);

	this->resetRequest();
	this->resetResponse();

	this->server_ = server;
	this->server_fd_ = server_fd;
}

Client::Client( const Client& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

Client::~Client( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

Client&	Client::operator=( const Client& rhs ) {

	if (this != &rhs) {
		this->fd_ = rhs.fd_;
		this->address_ = rhs.address_;
		this->latest_time_ = rhs.latest_time_;
		this->server_ = rhs.server_;
		this->server_fd_ = rhs.server_fd_;
		this->request_ = rhs.request_;
		this->response_ = rhs.response_;
	}
	return *this;
}

/* CLASS PUBLIC METHODS */

// setters
void	Client::setLatestTime( void ) {
	this->latest_time_ = time(0);
}

// getters
int	Client::getFd( void ) const {
	return this->fd_;
}

time_t	Client::getLatestTime( void ) const {
	return this->latest_time_;
}

Server*	Client::getServer( void ) const {
	return this->server_;
}

int	Client::getServerFd( void ) const {
	return this->server_fd_;
}

std::string	Client::getClientHost() const {

	char		client_host[INET_ADDRSTRLEN];
	
	inet_ntop(this->address_.sin_family, (struct sockaddr*)&this->address_, client_host, INET_ADDRSTRLEN);
	return client_host;
}

std::string	Client::getClientResponse( void ) {

	if (this->request_.getComplete())
		return this->response_.get();
	else
		return ("");
}

void	Client::addToRequest( std::string message ) {
	
	this->request_.add(message);
}

void	Client::resetResponse( void ) {
	
	this->response_.clear();
}

void	Client::resetRequest( void ) {

	this->request_.clear();
}


/* CLASS PRIVATE METHODS */