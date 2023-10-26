#include "ServerManager.hpp"
#include "Logger.hpp"
#include "TimeUtils.hpp"
#include "Color.hpp"

ServerManager::ServerManager( void ) {
	FD_ZERO(&this->master_fd_set_);
	FD_ZERO(&this->read_fd_set_);
	FD_ZERO(&this->write_fd_set_);
	this->servers_.clear();
	this->server_map_.clear();
	this->client_map_.clear();
	
}

ServerManager::ServerManager( const ServerManager& other ) {

}

ServerManager::~ServerManager( void ) {}

ServerManager&	ServerManager::operator=(const ServerManager& rhs) {

}

/*! \brief Get a server listening socket.
*         This is function takes a reference of the server
*			that needs a listening socket and returns either
*			a working listening socket or -1 in case of error.
*
*  This function proceeds in the following way:
*		1. 
*		
*
*/

// int	ServerManager::getServerListenerSocket( Server& server ) {
// 	int	listener;

// 	if (listener = socket(PF_INET, SOCK_STREAM, 0) == -1) {
// 		Logger::log(E_ERROR, "RED here", "server socket: %s", strerror(errno)); // make better later!
// 		return -1;
// 	}

// 	if (fcntl(listener, F_SETFL, O_NONBLOCK) == -1) {
// 		Logger::log(E_ERROR, "RED here", "server fcntl: %s", strerror(errno)); // make better later!
// 		close(listener);
// 		return -1;
// 	}

// 	server.getServerAddress().sin_family = AF_INET;
// 	server.getServerAddress().sin_port = htons(server.getListenerPort());
// 	server.getServerAddress().sin_addr.s_addr = server.getHost();
// 	memset(server.getServerAddress().sin_zero, '\0', sizeof server.getServerAddress().sin_zero);

// 	int	yes = 1;	// for setsockopt()
// 	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
// 	if (bind(listener, (struct sockaddr *)&server.getServerAddress(), sizeof server.getServerAddress()) == -1) {
// 		Logger::log(E_ERROR, "RED here", "server bind: %s", strerror(errno)); // make better later!
// 		close(listener);
// 		return -1;
// 	}
	
// 	if (listen(listener, LISTEN_BACKLOG) == -1) {
// 		Logger::log(E_ERROR, "RED here", "server listen: %s", strerror(errno)); // make better later!
// 		close(listener);
// 		return -1;
// 	}
// 	Logger::log(E_INFO, "WHITE here", "server ready!"); // make better later!
// 	return listener;
// }

/*! \brief Get a server listening socket.
*
*			THIS ONE USES ADDRINFO
*			also the port was a std::string
*
*         This is function takes a reference of the server
*			that needs a listening socket and returns either
*			a working listening socket or -1 in case of error.
*
*  This function proceeds in the following way:
*		1. Gets a pointer to addrinfo
*		2. keeps looping until successfully creates a socket and binds it
*		3. free addrinfo
*		4. check that bounding was successful
*		5. attempts to listen
*		6. returns listening socket descriptor
*
*/

// int	ServerManager::getServerListenerSocket( Server& server ) {
// 	int	listener;
// 	int	rv;
// 	int	setsockopt_option = 1;	// for setsockopt()

// 	struct addrinfo	hints, *addressInfo, *p;

// 	memset(&hints, 0, sizeof hints);
// 	hints.ai_family = AF_INET;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = htons(server.getHost());

// 	char	portString[server.getListenerPort().length()];		// convert from std::string to char*
// 	std::strcpy(portString, server.getListenerPort().c_str());

// 	if ((rv = getaddrinfo(NULL, portString, &hints, &addressInfo)) != 0) {
// 		std::cerr << "getListenerSocket: " << gai_strerror(rv) << std::endl;
// 		return -1;
// 	}

// 	for (p = addressInfo; p != NULL; p = p->ai_next) {

// 		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
// 		if (listener < 0)
// 			continue;
		
// 		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &setsockopt_option, sizeof(int));

// 		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
// 			close(listener);
// 			continue;
// 		}

// 		break;
// 	}

// 	freeaddrinfo(addressInfo);

// 	if (p == NULL)
// 		return -1;

// 	if (listen(listener, LISTEN_BACKLOG) == -1)
// 		return -1;

// 	return listener;
// }

void	ServerManager::closeServerSockets( void ) {
	Logger::log(E_INFO, COLOR_WHITE, "Closing all server sockets...");
	for (std::map<int, Server*>::iterator it = this->server_map_.begin(); it != this->server_map_.end(); ++it) {
		Logger::log(E_INFO, COLOR_WHITE, "Closing socket of %s", it->second->getServerIdforLog());
		close(it->first);
	}
}

void	ServerManager::closeClientSockets( void ) {
	Logger::log(E_INFO, COLOR_WHITE, "Closing all remaining client sockets...");
	for (std::map<int, Client>::iterator it = this->client_map_.begin(); it != this->client_map_.end(); ++it) {
		Logger::log(E_INFO, COLOR_WHITE, "Closing client %i connection", it->first);
		close(it->first);
	}
}

void	ServerManager::closeAllSockets( void ) {
	Logger::log(E_INFO, COLOR_WHITE, "Closing all sockets...");
	this->closeServerSockets();
	this->closeClientSockets();
}

bool	ServerManager::checkLastClientTime( void ) {
	time_t	now;

	for (std::map<int, Client>::iterator it = this->client_map_.begin(); it != this->client_map_.end(); ++it) {
		if (it->second.getLatestTime() > this->last_client_time_)
			this->last_client_time_ = it->second.getLatestTime();
	}
	if (time(&now) >= this->last_client_time_ + SERVER_SHUTDOWN_TIME_SEC)
		return true;
	return false;
}


/********************************************** SELECT functions **********************************************************/


/*! \brief Create server sockets (SELECT VERSION).
*         This function initiates all of the server
*			
*			
*			returns false in case of error
*
*  This function proceeds in the following way:
*		1.		SETUP ONE LISTENER SOCKET PER SERVER
*		1.1.	Make socket non-blocking
*		1.2.	Add a pollfd to pollfd-vector and set listening socket's socket descriptor as its fd (set event as POLLIN) 
*		...
*
*/

bool	ServerManager::SELECT_initializeServers( void ) {
	size_t	server_amount = this->servers_.size();

	int server_socket;

	for (int i = 0; i < server_amount; i++) {
		server_socket = this->servers_.at(i).setupServer();
		if (server_socket == -1)
			return false;

		
		FD_SET(server_socket, &this->master_fd_set_);	// add server socket to master_fd_set_
		FD_SET(server_socket, &this->read_fd_set_);		// add server socket to read_fd_set_

		this->server_map_[server_socket] = &this->servers_[i];	// add server socket to server_map_
	}

	this->biggest_fd_ = getBiggestFdOfSet(FD_SETSIZE - 1, &this->master_fd_set_);

	return true;
}

/*! \brief initialize fd_sets for running the servers. 
*			
*			d
*			
*
*  This function proceeds in the following way:
*		1.		
*		...
*
*/

void	ServerManager::SELECT_initializeFdSets( void ) {
	for (std::map<int, Server*>::iterator it = this->server_map_.begin(); it != this->server_map_.end(); ++it) {
		FD_SET(it->first, &this->read_fd_set_);
	}
}

/*! \brief run Servers; MAIN SERVER FUNCTION (SELECT VERSION).
*         This function handles the running and management of servers and their clients.
*			
*			
*			returns 0 on successful run and termination, else returns 1.
*
*  This function proceeds in the following way:
*		1.		
*		...
*
*/

int	ServerManager::SELECT_runServers( std::vector<Server>& server_vector ) {
	this->servers_ = server_vector;	// copy the vector of servers to the ServerManager
	if (!this->SELECT_initializeServers())	//initialize server sockets
		return 1;

	struct timeval	select_timeout;
	int				select_result;

	this->last_client_time_ = time(NULL);	// get the start time
	while (true) {	//	MAIN LOOP

		select_timeout.tv_sec = TIMEOUT_SEC;
		select_timeout.tv_usec = TIMEOUT_USEC;
	
		if ((select_result = select(this->biggest_fd_ + 1, &this->read_fd_set_, &this->write_fd_set_, NULL, &select_timeout)) == -1) {
			Logger::log(E_ERROR, COLOR_RED, "select: %s", strerror(errno));
		}
		// if (select_result == 0) // remove this? will never reach checkLastClientTime
		// 	continue;
		for (int i = 0; i <= this->biggest_fd_; ++i) {
			if (FD_ISSET(i, &this->read_fd_set_)) {
				if (this->server_map_.count(i)) {}
					// new connection!
				if (this->client_map_.count(i)) {}
					// client request
			}
			if (FD_ISSET(i, &this->write_fd_set_)) {
				if (this->server_map_.count(i)) {}
					// server receiving stuff from client?
				if (this->client_map_.count(i)) {}
					// client response

			}
		}

		if (this->checkLastClientTime())	// if the haven't been any client activity in the server shutdown time end run
			break;
	}
	Logger::log(E_INFO, COLOR_GREEN, "Successful end; server shutting down...");
	this->closeAllSockets();
	return 0;
}

void	ServerManager::SELECT_acceptNewClientConnection( int server_fd ) {
	int			client_fd;
	sockaddr_in client_address;
	socklen_t	address_size;
	Server*		server = this->server_map_.at(server_fd);
	
	address_size = sizeof(sockaddr_in);
	if ((client_fd = accept(server_fd, (struct sockaddr*)&client_address, &address_size)) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "accept: %s, tried to connect to server on port ", strerror(errno), server->getListenerPort());
		return;
	}
	Logger::log(E_INFO, COLOR_BLUE, "New connection to server %s on port %i, client assigned to socket %s", server->getServerName(), server->getListenerPort(), client_fd);

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fcntl error: %s, client %d rejected", client_fd);
		return;
	}

	this->client_map_.insert(std::make_pair(client_fd, Client(server_fd, server)));

	FD_SET(client_fd, &this->master_fd_set_);
	FD_SET(client_fd, &this->write_fd_set_); // because of request? talk with Jenny
}

/********************************************** POLL functions **********************************************************/


/*! \brief Create server sockets (POLL VERSION).
*         This function creates the server listener sockets.
*			
*			
*			returns false in case of error
*
*  This function proceeds in the following way:
*		1.		SETUP ONE LISTENER SOCKET PER SERVER
*		1.1.	Make socket non-blocking
*		1.2.	Add a pollfd to pollfd-vector and set listening socket's socket descriptor as its fd (set event as POLLIN) 
*		...
*
*/

bool	ServerManager::POLL_initializeServers() {
	int	newfd;	// for newly accept()ed socket descriptors
	struct sockaddr_storage	remoteaddr; // client address, may not be needed
	

	size_t	serverAmount = this->servers_.size();	// get the amount of servers

	int listenerSockets[serverAmount]; // an array for the listener socket descriptors

	for (int i = 0; i < serverAmount; i++) {	// set up listener sockets
		listenerSockets[i] = this->servers_.at(i).setupServer();	// get listener socket
		if (listenerSockets[i] == -1)
			return false;

		if (fcntl(listenerSockets[i], F_SETFL, O_NONBLOCK) == -1) {	// make socket non-blocking
			perror("fcntl");
			return false;
		}
		
		this->pollfds_.push_back(pollfd{listenerSockets[i], POLLIN});	// push a new pollfd into the pollfds_ -vector
		this->server_map_[listenerSockets[i]] = &this->servers_[i];		// add fd and the server to the server_map_
	}
	return true;
}

/********************************************** NOT CLASS functions **********************************************************/

int	getBiggestFdOfSet( int max_fd_size, fd_set* set ) {
	for (int fd = max_fd_size; fd >= 0; --fd) {
        if (FD_ISSET(fd, set)) {
            return fd;
        }
    }
    return -1;
}