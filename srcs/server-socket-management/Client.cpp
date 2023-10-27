#include "Client.hpp"

/* CONSTRUCTORS */

Client::Client( void ) {}

Client::Client( int server_fd, Server* server ) {
	this->server_fd_ = server_fd;
	this->server_ = server;
}

// Client::Client( const Client& to_copy ) {

// 	/* copy constructor */
// } 

/* DESTRUCTOR */

Client::~Client( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

// Client&	Client::operator=( const Client& rhs ) {

// 	/* copy assignment operator overload */
// }

/* CLASS PUBLIC METHODS */

// setters
// int	Client::setupClient( void ) {

// }

void	Client::setLatestTime( void ) {

}

// getters
int	Client::getFd( void ) const {
	return this->fd_;
}

// struct sockaddr_in&	Client::getAddress( void ) const {
// 	return this->address_;
// }

time_t	Client::getLatestTime( void ) const {
	return this->latest_time_;
}

Server*	Client::getServer( void ) const {
	return this->server_;
}

int	Client::getServerFd( void ) const {
	return this->server_fd_;
}

std::string	Client::getFullClientIdForLog() const {
	char		client_ip[INET_ADDRSTRLEN];
	std::string	id;

	id += "CLIENT[SOCKET: ";
	id += this->fd_;
	id += ", IP: ";
	id += inet_ntop(this->address_.sin_family, (struct sockaddr*)&this->address_, client_ip, INET_ADDRSTRLEN);
	id += "]";

	return id;
}

/* CLASS PRIVATE METHODS */