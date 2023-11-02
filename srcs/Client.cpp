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
	this->latest_time_ = time(0);
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

	// std::cout << "in addToRequest" << std::endl;
	
	this->request_.add(message);
}

void	Client::resetResponse( void ) {
	
	this->response_.clear();
}

void	Client::resetRequest( void ) {

	this->request_.clear();
}


/* CLASS PRIVATE METHODS */