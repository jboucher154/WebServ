#ifndef	SOCKETS_HPP
#define	SOCKETS_HPP

//	talk with others about if other header files have these in them!
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <utility>
#include <map>
#include <vector>

#include "Color.hpp"
#include "Logger.hpp"

// LIBRARIES NEEDED FOR SOCKETS

/*
Needed for converting std::string type to char *.
*/
#include <cstring>

/*
Needed for fcntl().
*/
#include <fcntl.h>

/*
CHECK LATER
needed for POLLIN.
*/
#include <poll.h>

/*
Provides functions for handling Internet addresses, 
specifically for conversions between numeric IP addresses and human-readable 
string representations.
*/
#include <arpa/inet.h>

/*
Socket Programming Library.
Provides functions and structures for creating and using sockets.
*/
#include <sys/socket.h>

/*
Internet Protocol (IP) Socket Library.
Defines structures like sockaddr_in and constants for socket-related operations.
*/
#include <netinet/in.h>

/*
Network Database Library.
Defines structure addrinfo and functions getaddinfo() and freeaddrinfo().

Don't need currently
*/
#include <netdb.h>


// MACROS
/*
Number of connections allowed on the incoming queu
*/
#define	LISTEN_BACKLOG	20


// REMOVE LATER; CREATED FOR TESTING
class Server {
private:
	uint16_t			listening_port_;
	in_addr_t			host_;
	std::string			server_name_;
	struct sockaddr_in			server_address_;

	// the rest of the data like locations etc...

public:
	Server();
	Server(int port, std::string  ipAddress, std::string serverName);
	~Server();

	uint16_t			getListenerPort();
	in_addr_t			getHost();
	std::string			getServerName();
	struct sockaddr_in&	getServerAddress();
	std::string			getServerIdforLog() const;

	int	setupServer();
	
};

Server::Server() {
	this->listening_port_ = 8080;
	this->host_ = inet_addr(std::string("127.0.0.1").data());
	this->server_name_ = "Default Server";
}

Server::Server(int port, std::string  ipAddress, std::string serverName) {
	this->listening_port_ = port;
	this->host_ = inet_addr(ipAddress.data());
	this->server_name_ = serverName;
}

Server::~Server() {}


/*! \brief Setup a server listener socket
*         This is function sets up a working listening socket
*			and returns the file (socket) descriptor.
*			If error returns -1.
*			Be aware: this is the old method of setting up a socket address, but
*			there is not really a need for addrinfo-method because we only use IPv4.
*			
*
*  longer description here.
*
*/

int	Server::setupServer( void ) {
	int	listener_fd;
	int	yes = 1;	// for setsockopt()

	if ((listener_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "socket failure: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		return -1;
	}

	if (fcntl(listener_fd, F_SETFL, O_NONBLOCK) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fcntl failure: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		close(listener_fd);
		return -1;
	}

	this->server_address_.sin_family = AF_INET;
	this->server_address_.sin_port = htons(this->listening_port_);
	this->server_address_.sin_addr.s_addr = this->host_;
	memset(this->server_address_.sin_zero, '\0', sizeof this->server_address_.sin_zero);

	setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));	// supresses error message if port is reused

	if (bind(listener_fd, (struct sockaddr *)&this->server_address_, sizeof this->server_address_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "bind failure: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		close(listener_fd);
		return -1;
	}

	if (listen(listener_fd, LISTEN_BACKLOG) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "listen failure: %s, %s", strerror(errno), this->getServerIdforLog().c_str());
		close(listener_fd);
		return -1;
	}
	Logger::log(E_INFO, COLOR_WHITE, "%s ready!", this->getServerIdforLog().c_str());
	return listener_fd;
}

std::string	Server::getServerIdforLog( void ) const {
	std::string	id;

	id += "SERVER[NAME: ";
	id += this->server_name_;
	id += ", HOST: ";
	id += this->host_;
	id += ", PORT: ";
	id += this->listening_port_;
	id += "]"; 
	return id;
}

#endif