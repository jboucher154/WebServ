#include "ServerManager.hpp"
#include "Logger.hpp"
#include "TimeUtils.hpp"
#include "Color.hpp"

#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

ServerManager::ServerManager( void ) {
	
	this->servers_.clear();

	FD_ZERO(&this->read_fd_set_);
	FD_ZERO(&this->write_fd_set_);
	this->biggest_fd_ = -1;

	this->pollfds_.clear();
	this->pollfds_size_ = -1;

	this->server_map_.clear();
	this->client_map_.clear();
}

ServerManager::ServerManager( std::vector<Server>& server_vector ) {

	this->servers_.clear();

	FD_ZERO(&this->read_fd_set_);
	FD_ZERO(&this->write_fd_set_);
	this->biggest_fd_ = -1;

	this->pollfds_.clear();
	this->pollfds_size_ = -1;

	this->server_map_.clear();
	this->client_map_.clear();

	this->servers_ = server_vector;
}

ServerManager::ServerManager( const ServerManager& other ) {

	*this = other;
}

ServerManager::~ServerManager( void ) {}

ServerManager&	ServerManager::operator=(const ServerManager& rhs) {

	if (this != &rhs) {
		this->servers_ = rhs.servers_;
		this->read_fd_set_ = rhs.read_fd_set_;
		this->write_fd_set_ = rhs.write_fd_set_;
		this->biggest_fd_ = rhs.biggest_fd_;
		this->pollfds_ = rhs.pollfds_;
		this->pollfds_size_ = rhs.pollfds_size_;
		this->server_map_ = rhs.server_map_;
		this->client_map_ = rhs.client_map_;
		this->last_client_time_ = rhs.last_client_time_;
	}
	return *this;
}



void	ServerManager::closeServerSockets( void ) {

	Logger::log(E_INFO, COLOR_WHITE, "Closing all server sockets...");

	for (std::map<int, Server*>::iterator it = this->server_map_.begin(); it != this->server_map_.end(); ++it) {
		Logger::log(E_INFO, COLOR_WHITE, "Closing socket of %s", it->second->getServerIdforLog().c_str());
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

void	ServerManager::removeClient( int client_fd ) {

	Logger::log(E_INFO, COLOR_MAGENTA, "Socket %d connection lost, clearing client data...", client_fd);
	close(client_fd);
	this->client_map_.erase(client_fd);
}


bool	ServerManager::receiveFromClient( int client_fd ) {
	
	char		client_msg[4000];
	Client*		client = &this->client_map_[client_fd];
	Server*		server = client->getServer();
	Request&	request = client->getRequest();

	client->setLatestTime();
	memset(client_msg, 0, 4000);
	int bytes_received = recv(client_fd, &client_msg, 3999, 0);

	// std::cout << "bytes received: " << bytes_received << std::endl;
	// std::cout << "messge:  " << client_msg << std::endl;

	if (bytes_received == -1)
		Logger::log(E_ERROR, COLOR_RED, "recv error, from socket %d to server %s",
			client_fd, server->getServerIdforLog().c_str());
	else if (bytes_received == 0)	// client has disconnected...
		return false;
	else {
		client->addToRequest(client_msg);
		Logger::log(E_INFO, COLOR_WHITE, "server %s receives request from socket %d, METHOD=<%s>, URI=<%s>",
			server->getServerName().c_str(), client_fd, request.getRequestLineValue("method").c_str(), request.getRequestLineValue("uri").c_str());
	}

	return true;
}


bool	ServerManager::sendResponseToClient( int client_fd ) {

	Client*		client = &this->client_map_[client_fd];
	Server*		server = client->getServer();
	Response&	response = client->getResponse();	

	bool	keep_alive = client->getRequest().getKeepAlive();

	std::string	response_string = client->getResponseString();
	if (response_string.empty())
		return keep_alive;
	(void)response;//?
	client->setLatestTime();
	int	bytes_sent = send(client_fd, response_string.c_str(), response_string.length(), 0);
	if (bytes_sent == -1) {
		Logger::log(E_ERROR, COLOR_RED, "send error, from server %s to socket %d",
			server->getServerIdforLog().c_str(), client_fd);
	}
	else if (bytes_sent == 0) {
		Logger::log(E_DEBUG, COLOR_YELLOW, " Server %s sent 0 bytes to socket %d",
			server->getServerIdforLog().c_str(), client_fd);
	}
	else {
		if (bytes_sent < static_cast<int>(response_string.length()))
			Logger::log(E_ERROR, COLOR_RED, "incomplete response sent from server %s to socket %d ",
				server->getServerIdforLog().c_str(), client_fd);
		else
			Logger::log(E_INFO, COLOR_WHITE, "server %s sent response to socket %d, STAT=<%d>",
				server->getServerName().c_str(), client_fd, -42);										// ADD STAT CODE WHEN JENNY MAKES GETSTAT!
	}
	// if bytes_sent == 0 do something?

	client->resetResponse();
	client->resetRequest();

	return keep_alive;
}


/********************************************** SELECT functions **********************************************************/

bool	ServerManager::SELECT_initializeServers( void ) {

	int	server_amount = this->servers_.size();
	int server_socket;

	Logger::log(E_INFO, COLOR_BRIGHT_GREEN, "Initializing servers...");

	for (int i = 0; i < server_amount; i++) {
		server_socket = this->servers_.at(i).setupServer();
		if (server_socket == -1) {
			this->closeServerSockets();
			return false;
		}

		FD_SET(server_socket, &this->read_fd_set_);		// add server socket to read_fd_set_

		this->server_map_[server_socket] = &this->servers_[i];	// add server socket to server_map_
	}

	this->biggest_fd_ = SELECT_getBiggestFd(FD_SETSIZE - 1);	// get the biggest fd of the read set

	Logger::log(E_INFO, COLOR_BRIGHT_GREEN, "initialization of servers is complete!");

	return true;
}

int		ServerManager::SELECT_getBiggestFd( int max_fd_size ) {

	for (int fd = max_fd_size; fd >= 0; --fd) {
        if (FD_ISSET(fd, &this->read_fd_set_) || FD_ISSET(fd, &this->write_fd_set_))
            return fd;
    }
    return -1;	
}

bool	ServerManager::SELECT_runServers( void ) {

	struct timeval	select_timeout;
	int				select_result;
	fd_set			read_fd_set_copy;
	fd_set			write_fd_set_copy;

	Logger::log(E_INFO, COLOR_GREEN, "runServers() starting...");

	this->last_client_time_ = time(0);	// get the start time

	while (true) {	//	MAIN LOOP

		this->SELECT_runServersLoopStart(select_timeout, read_fd_set_copy, write_fd_set_copy);	//ready everything for select

		this->SELECT_printSetData();	// instead of commenting this out just set the GET_DEBUG_LOG macro to false
	
		if ((select_result = select(this->biggest_fd_ + 1, &read_fd_set_copy, &write_fd_set_copy, NULL, &select_timeout)) == -1) {
			Logger::log(E_ERROR, COLOR_RED, "SELECT ERROR: %s, [WHAT ARE THE CHANCES?!]", strerror(errno));
			this->closeAllSockets();
			return false;
		}
	
		for (int fd = 0; fd <= this->biggest_fd_; ++fd) {
			if (FD_ISSET(fd, &read_fd_set_copy)) {
				if (this->server_map_.count(fd))
					this->SELECT_acceptNewClientConnection(fd);	// new client connection
				if (this->client_map_.count(fd))
					this->SELECT_receiveFromClient(fd);				// client request or disconnection
			}
			if (FD_ISSET(fd, &write_fd_set_copy)) {
				if (this->server_map_.count(fd)) {}
					// server receiving stuff from client?		// I don't think anything happens here, remove
				if (this->client_map_.count(fd))
					this->SELECT_sendResponseToClient(fd);				// send a response to client
			}
		}
	
		if (this->checkLastClientTime())	// if the haven't been any client activity in the server shutdown time end run
			break;
	}

	Logger::log(E_INFO, COLOR_GREEN, "Server run timeout; servers shutting down... [THE GOOD AND PROPER ENDING]");
	this->closeAllSockets();
	return true;
}

// this function is called in the beginning of the runServer Loop.
// 1. sets timeout values
// 2. copy the Webserver's sets to the pointed to sets
// 3.  get the biggest fd in the master_fd_set_
// expand on this more later!
void	ServerManager::SELECT_runServersLoopStart( timeval& select_timeout, fd_set& read_fd_set_copy, fd_set& write_fd_set_copy ) {

	select_timeout.tv_sec = SELECT_TIMEOUT_SEC;
	select_timeout.tv_usec = SELECT_TIMEOUT_USEC;

	read_fd_set_copy = this->read_fd_set_;
	write_fd_set_copy = this->write_fd_set_;

	this->biggest_fd_ = SELECT_getBiggestFd(FD_SETSIZE - 1);
}


void	ServerManager::SELECT_acceptNewClientConnection( int server_fd ) {

	int			client_fd;
	sockaddr_in client_address;
	socklen_t	client_address_size;
	char		client_host[INET_ADDRSTRLEN];
	Server*		server = this->server_map_.at(server_fd);
	
	client_address_size = sizeof(sockaddr_in);
	if ((client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_size)) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "accept error: %s, tried to connect to server on port %d", strerror(errno), server->getListeningPortInt());
		return;
	}

	Logger::log(E_INFO, COLOR_BRIGHT_BLUE, "New connection to server %s on port %d: assigned to socket %d [client host: %s]",
		server->getServerName().c_str(), server->getListeningPortInt(), client_fd, inet_ntop(client_address.sin_family, (struct sockaddr*)&client_address, client_host, INET_ADDRSTRLEN));

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fcntl error: %s, socket %d connection rejected", client_fd);
		close(client_fd);
		return;
	}

	this->client_map_.insert(std::make_pair(client_fd, Client(server_fd, server)));
	this->client_map_[client_fd].setLatestTime();

	FD_SET(client_fd, &this->read_fd_set_);
}

void	ServerManager::SELECT_removeFdFromSets( int fd ) {
	
	if (FD_ISSET(fd, &this->read_fd_set_))
		FD_CLR(fd, &this->read_fd_set_);
	if (FD_ISSET(fd, &this->write_fd_set_))
		FD_CLR(fd, &this->write_fd_set_);
		if (fd == this->biggest_fd_)
			this->biggest_fd_ = SELECT_getBiggestFd(FD_SETSIZE - 1);
}

void	ServerManager::SELECT_removeClient( int client_fd ) {

	this->SELECT_removeFdFromSets(client_fd);
	this->removeClient(client_fd);
}

void	ServerManager::SELECT_switchClientToReadSet( int client_fd ) {

	if (FD_ISSET(client_fd, &this->write_fd_set_))
		FD_CLR(client_fd, &this->write_fd_set_);
	if (!FD_ISSET(client_fd, &this->read_fd_set_))
		FD_SET(client_fd, &this->read_fd_set_);
}

void	ServerManager::SELECT_switchClientToWriteSet( int client_fd ) {

	if (FD_ISSET(client_fd, &this->read_fd_set_))
		FD_CLR(client_fd, &this->read_fd_set_);
	if (!FD_ISSET(client_fd, &this->write_fd_set_))
		FD_SET(client_fd, &this->write_fd_set_);
}

/*	check the following lines, maybe scuffed

if (client.getRequest().getCgiFlag() && client.getResponse().getStatusCode() < 400) {
			if ((client.POLL_startCgiResponse()) == true)
				this->POLL_addCgiFdsToPollfds_(client);
			return;

*/
void	ServerManager::SELECT_receiveFromClient( int client_fd ) {

	Client& client = this->client_map_[client_fd];

	if (client.getRequest().getCgiFlag()) {
		client.SELECT_finishCgiResponse();
		this->SELECT_removeCgiFdsFromSets_(client);
		this->SELECT_switchClientToWriteSet(client_fd);
		return;
	}

	if (this->receiveFromClient(client_fd))
		this->SELECT_removeClient(client_fd);
	else {
		client.getResponse().generate(&client.getRequest());
		if (client.getRequest().getCgiFlag() && client.getResponse().getStatusCode() < 400) {
			if ((client.SELECT_startCgiResponse()) == true)
				this->SELECT_addCgiFdsToSets_(client);
			return;
		}
		this->SELECT_switchClientToWriteSet(client_fd);
	}
}

/*	Use this function in the version that uses SELECT:
	Will call the receiveFromClient function but handle it's
	results in the correct SELECT way */
void	ServerManager::SELECT_sendResponseToClient( int client_fd ) {

	if (this->sendResponseToClient(client_fd))	// keep alive
		this->SELECT_switchClientToReadSet(client_fd);
	else	// don't keep alive
		this->SELECT_removeClient(client_fd);
}

void	ServerManager::SELECT_printSetData( void ) {
	
	std::string	all_fds = "all fds currently handled: ";
	std::string read_set = "read_fd_set: ";
	std::string write_set = "write_fd_set: ";

	std::string	fd_as_string;
	int			fds_amount = 0;

	for (int i = biggest_fd_; i >= 0; --i) {
		if (FD_ISSET(i, &this->read_fd_set_)) {
			fd_as_string = intToString(i);
			all_fds += fd_as_string + " ";
			read_set += fd_as_string + " ";
			++fds_amount;
		}
		if (FD_ISSET(i, &this->write_fd_set_)) {
			fd_as_string = intToString(i);
			all_fds += fd_as_string + " ";
			write_set += fd_as_string + " ";
			++fds_amount;
		}
	}

	Logger::log(E_DEBUG, COLOR_YELLOW, "fds amount: %d", fds_amount);
	Logger::log(E_DEBUG, COLOR_YELLOW, "the biggest fd currently: %d", this->biggest_fd_);
	Logger::log(E_DEBUG, COLOR_YELLOW, all_fds.c_str());
	Logger::log(E_DEBUG, COLOR_YELLOW, read_set.c_str());
	Logger::log(E_DEBUG, COLOR_YELLOW, write_set.c_str());
}

/********************************************** POLL functions **********************************************************/

bool	ServerManager::POLL_initializeServers( void ) {
	
	int		server_amount = this->servers_.size();
	int 	server_socket;

	Logger::log(E_INFO, COLOR_BRIGHT_GREEN, "initializing servers...");

	for (int i = 0; i < server_amount; i++) {
		server_socket = this->servers_.at(i).setupServer();
		if (server_socket == -1) {
			this->closeServerSockets();
			return false;
		}

		pollfd new_pollfd = {server_socket, POLLIN, 0};
		this->pollfds_.push_back(new_pollfd);					// push server socket to pollfds_ vector with POLLIN as event

		this->server_map_[server_socket] = &this->servers_[i];	// add server socket and the server itself to server_map_
	}

	this->pollfds_size_ = this->pollfds_.size();

	Logger::log(E_INFO, COLOR_BRIGHT_GREEN, "initialization of servers is complete!");

	return true;
}

bool	ServerManager::POLL_runServers( void ) {

	int				poll_result;

	Logger::log(E_INFO, COLOR_GREEN, "runServers() starting...");

	this->last_client_time_ = time(0);	// get the start time

	while (true) {	//	MAIN LOOP

		this->POLL_printData();	// instead of commenting this out just set the GET_DEBUG_LOG macro to false
	
		if ((poll_result = poll(&this->pollfds_[0], this->pollfds_.size(), POLL_TIMEOUT_MILLISEC)) == -1) {
			Logger::log(E_ERROR, COLOR_RED, "POLL ERROR: %s, [WHAT ARE THE CHANCES?!]", strerror(errno));
			this->closeAllSockets();
			return false;
		}

		/*	This i is needed to keep track of the size of the pollfds_ vector so that if a client disconnects
			the iterator will not go past the end of the vector. While erasing a pollfd in the vector will
			make it skip the next pollfd (the next pollfd takes the spot of the just deleted pollfd), it won't be
			a problem because of the looping. */
		this->pollfds_size_ = this->pollfds_.size();
		int	i = 0;

		for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); i < this->pollfds_size_ && it != this->pollfds_.end(); ++it, ++i) {
			if (it->revents & POLLIN) {
				if (this->server_map_.count(it->fd))
					this->POLL_acceptNewClientConnection(it->fd);
				if (this->client_map_.count(it->fd)) {
					this->POLL_receiveFromClient(it->fd);
					//generate
					//cgi finish
				}

			}
			if (it->revents & POLLOUT) {
				if (this->client_map_.count(it->fd))			// this is most likely not needed, server don't ever POLLOUT
					this->POLL_sendResponseToClient(it->fd);
			}
		}
	
		if (this->checkLastClientTime())	// if there hasn't been any client activity in the server shutdown time, end run
			break;
	}

	Logger::log(E_INFO, COLOR_GREEN, "Server run timeout; servers shutting down... [THE GOOD AND PROPER ENDING]");
	this->closeAllSockets();
	return true;
}

void	ServerManager::POLL_acceptNewClientConnection( int server_fd ) {

	int			client_fd;
	sockaddr_in client_address;
	socklen_t	client_address_size;
	char		client_host[INET_ADDRSTRLEN];
	Server*		server = this->server_map_.at(server_fd);
	
	client_address_size = sizeof(sockaddr_in);
	if ((client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_size)) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "accept error: %s, tried to connect to server on port %d", strerror(errno), server->getListeningPortInt());
		return;
	}

	Logger::log(E_INFO, COLOR_BRIGHT_BLUE, "New connection to server %s on port %d: assigned to socket %d [client host: %s]",
		server->getServerName().c_str(), server->getListeningPortInt(), client_fd, inet_ntop(client_address.sin_family, (struct sockaddr*)&client_address, client_host, INET_ADDRSTRLEN));

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fcntl error: %s, socket %d connection rejected", client_fd);
		close(client_fd);
		return;
	}

	this->client_map_.insert(std::make_pair(client_fd, Client(server_fd, server)));
	this->client_map_[client_fd].setLatestTime();

	pollfd new_pollfd = {client_fd, POLLIN, 0};
	this->pollfds_.push_back(new_pollfd);		// push a new pollfd into pollfds_ vector
}

void	ServerManager::POLL_removeFdFromPollfds( int fd ) {

	for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); it != this->pollfds_.end(); ++it) {
		if (it->fd == fd) {
			this->pollfds_.erase(it);
			break;
		}
	}
	this->pollfds_size_--;
}

void	ServerManager::POLL_removeClient( int client_fd ) {

	this->POLL_removeFdFromPollfds(client_fd);
	this->removeClient(client_fd);
}


void	ServerManager::POLL_switchClientToPollin ( int client_fd ) {

	for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); it != this->pollfds_.end(); ++it) {
		if (it->fd == client_fd) {
			it->events = POLLIN;
			break;
		}
	}
}


void	ServerManager::POLL_switchClientToPollout( int client_fd ) {

	for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); it != this->pollfds_.end(); ++it) {
		if (it->fd == client_fd) {
			it->events = POLLOUT;
			break;
		}
	}
}

/*	check the following lines, maybe scuffed

if (client.getRequest().getCgiFlag() && client.getResponse().getStatusCode() < 400) {
			if ((client.POLL_startCgiResponse()) == true)
				this->POLL_addCgiFdsToPollfds_(client);
			return;

*/
void	ServerManager::POLL_receiveFromClient( int client_fd ) {

	Client&	client = this->client_map_[client_fd];

	if (client.getRequest().getCgiFlag()) {
		client.POLL_finishCgiResponse();
		this->POLL_removeCgiFdsFromPollfds_(client);
		this->POLL_switchClientToPollout(client_fd);
		return;
	}
	
	if (!this->receiveFromClient(client_fd))
		this->POLL_removeClient(client_fd);
	else {
		client.getResponse().generate(&client.getRequest());
		if (client.getRequest().getCgiFlag() && client.getResponse().getStatusCode() < 400) {
			if ((client.POLL_startCgiResponse()) == true)
				this->POLL_addCgiFdsToPollfds_(client);
			return;
		}
		this->POLL_switchClientToPollout(client_fd);
	}
}


void	ServerManager::POLL_sendResponseToClient( int client_fd ) {

	if (this->sendResponseToClient(client_fd))	// keep alive 
		this->POLL_switchClientToPollin(client_fd);
	else	// don't keep alive
		this->POLL_removeClient(client_fd);
}


void	ServerManager::POLL_printData( void ) {

	std::string	all_fds = "all pollfds currently handled: ";
	std::string pollin_fds = "pollfds with POLLIN: ";
	std::string pollout_fds = "pollfds with POLLOUT: ";

	std::string	fd_as_string;

	for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); it != this->pollfds_.end(); ++it) {
		fd_as_string = intToString(it->fd) + " ";
		all_fds += fd_as_string;
		if (it->events == POLLIN)
			pollin_fds += fd_as_string;
		if (it->events == POLLOUT)
			pollout_fds += fd_as_string;
	}

	Logger::log(E_DEBUG, COLOR_YELLOW, "the amount of pollfds: %d", this->pollfds_.size());
	Logger::log(E_DEBUG, COLOR_YELLOW, all_fds.c_str());
	Logger::log(E_DEBUG, COLOR_YELLOW, pollin_fds.c_str());
	Logger::log(E_DEBUG, COLOR_YELLOW, pollout_fds.c_str());
}

/* CLASS PRIVATE METHODS */

void	ServerManager::POLL_addCgiFdsToPollfds_( Client& client ) {

	std::vector<pollfd>& cgi_pollfds = client.getCgiHandler()->getCgiPollfds();

	for (std::vector<pollfd>::iterator it = cgi_pollfds.begin(); it != cgi_pollfds.end(); ++it) {
		this->pollfds_.push_back(*it);
		++this->pollfds_size_;
	}
}

void	ServerManager::POLL_removeCgiFdsFromPollfds_( Client& client ) {

	std::vector<pollfd>& cgi_pollfds = client.getCgiHandler()->getCgiPollfds();

	for (std::vector<pollfd>::iterator it = cgi_pollfds.begin(); it != cgi_pollfds.end(); ++it) {
		this->POLL_removeFdFromPollfds(it->fd);
	}

	client.getCgiHandler()->getCgiPollfds();
}


void	ServerManager::SELECT_addCgiFdsToSets_( Client& client ) {

	fd_set&	cgi_read_set = client.getCgiHandler()->getCgiReadSet();
	fd_set&	cgi_write_set = client.getCgiHandler()->getCgiwriteSet();

	int	hits = 0;
	for (int fd = FD_SETSIZE - 1; fd >= 0; --fd) {
		if (FD_ISSET(fd, &cgi_read_set)) {
			FD_SET(fd, &cgi_read_set);
			++hits;
		}
		if (FD_ISSET(fd, &cgi_write_set)) {
			FD_SET(fd, &cgi_write_set);
			++hits;
		}
		if (hits >= 2)
			break;
	}

	this->biggest_fd_ = this->SELECT_getBiggestFd(FD_SETSIZE - 1);
}

void	ServerManager::SELECT_removeCgiFdsFromSets_( Client& client ) {

	fd_set&	cgi_read_set = client.getCgiHandler()->getCgiReadSet();
	fd_set&	cgi_write_set = client.getCgiHandler()->getCgiwriteSet();

	int	hits = 0;
	for (int fd = FD_SETSIZE - 1; fd >= 0; --fd) {
		if (FD_ISSET(fd, &cgi_read_set)) {
			FD_CLR(fd, &this->read_fd_set_);
			++hits;
		}
		if (FD_ISSET(fd, &cgi_write_set)) {
			FD_SET(fd, &this->write_fd_set_);
			++hits;
		}
		if (hits >= 2)
			break;
	}

	client.getCgiHandler()->clearCgiHandlerPollfdsAndSets();
}