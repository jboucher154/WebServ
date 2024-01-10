#include "ServerManager.hpp"
#include "Logger.hpp"
#include "utility.hpp"
#include "Color.hpp"

#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

/*! \brief Construct a new Server Manager:: Server Manager object.
 * 
 * @param server_vector a reference to the Validator's vector of servers.
 */
ServerManager::ServerManager( std::vector<Server>& server_vector ) : servers_(server_vector) {

	this->servers_.clear();

	#if POLL_TRUE_SELECT_FALSE
		this->pollfds_.clear();
		this->pollfds_size_ = -1;
	#else
		FD_ZERO(&this->read_fd_set_);
		FD_ZERO(&this->write_fd_set_);
		this->biggest_fd_ = -1;
	#endif

	this->server_map_.clear();
	this->client_map_.clear();
}

/*! \brief Copy construct a new Server Manager:: Server Manager object
 *
 * NOT USED ANYWHERE.
 * 
 * @param other the other ServerManager
 */
ServerManager::ServerManager( const ServerManager& other ) : servers_(other.servers_) {

	*this = other;
}

/*! \brief Destroy the Server Manager:: Server Manager object
 *	
 */
ServerManager::~ServerManager( void ) {

}

/*! \brief ServerManager's assignment operator overload
*       
*  Returns a pointer to a copy of current instance. 
*
*   @param rhs, a  reference to an instance of the ServerManager class.
*   @return a  reference to an instance of the ServerManager class created(copied) out of rhs.
*/
ServerManager&	ServerManager::operator=(const ServerManager& rhs) {

	if (this != &rhs) {
		this->servers_ = rhs.servers_;

		#if POLL_TRUE_SELECT_FALSE
			this->pollfds_ = rhs.pollfds_;
			this->pollfds_size_ = rhs.pollfds_size_;
		#else
			this->read_fd_set_ = rhs.read_fd_set_;
			this->write_fd_set_ = rhs.write_fd_set_;
			this->biggest_fd_ = rhs.biggest_fd_;
		#endif

		this->server_map_ = rhs.server_map_;
		this->client_map_ = rhs.client_map_;
		this->latest_server_time_ = rhs.latest_server_time_;
	}
	return *this;
}


/*! \brief get a client by it's cgi pipe fd.
 *
 *	This function is called when the ServerManager detects activity in a fd with a write flag which is NOT a client fd.
 *	In the function we go through the client_cgi_map_ and see which of client has the pipe_fd as one of its values.
 * 
 *	@param pipe_fd the fd of a pipe end which triggered this function to be called.
 *	@return int which is the client fd (or -1 in case of error).
 */
int	ServerManager::getClientFdByItsCgiPipeFd( int pipe_fd ) {

	for (std::map<int, std::vector<int> >::iterator it = this->client_cgi_map_.begin(); it != this->client_cgi_map_.end(); ++it) {
		if (it->second[0] == pipe_fd || it->second[1] == pipe_fd)
			return it->first;
	}
	
	return -1;
}

/*! \brief Close server sockets.
 * 		This function is called after the SERVER_SHUTDOWN_TIME_SEC has been passed without any client activity.
 */
void	ServerManager::closeServerSockets( void ) {

	Logger::log(E_INFO, COLOR_WHITE, "Closing all server sockets...");

	for (std::map<int, Server*>::iterator it = this->server_map_.begin(); it != this->server_map_.end(); ++it) {
		Logger::log(E_INFO, COLOR_WHITE, "Closing socket of %s", it->second->getServerIdforLog().c_str());
		close(it->first);
	}
}

/*! \brief Close all remaining client connections.
 *		Called after the SERVER_SHUTDOWN_TIME_SEC has passed withouth any client activity.
 * 
 */
void	ServerManager::closeAllClientConnections( void ) {

	Logger::log(E_INFO, COLOR_WHITE, "Closing all remaining client sockets...");

	while(!this->client_map_.empty()) {
		#if POLL_TRUE_SELECT_FALSE
			this->POLL_removeClient(this->client_map_.begin()->first);
		#else
			this->SELECT_removeClient(this->client_map_.begin()->first);
		#endif
	}
}

/** \brief Close all sockets.
 * 		This function is called after the SERVER_SHUTDOWN_TIME_SEC has been passed without any client activity.
 * 
 */
void	ServerManager::closeAllSockets( void ) {

	Logger::log(E_INFO, COLOR_WHITE, "Closing all sockets...");

	this->closeAllClientConnections();
	this->closeServerSockets();
}

/*! \brief This function checks if the servers have been without activity past a certain deadline (SERVER_SHUTDOWN_TIME_SEC).
 * 		If this is the case we end running of the servers and close the program.
 * 
 *	The function iterates through all the servers and checks the latest client event in that server. We get the shortest time
 *	and we compare that to the SERVER_SHUTDOWN_TIME_SEC.
 * 
 *	@return true if the SERVER_SHUTDOWN_TIME_SEC has been passed (the program will be closed).
 *	@return false if the SERVER_SHUtDOWN_TIME_SEC has not been passed.
 */
bool	ServerManager::CheckServersTimeout( void ) {

	time_t	server_last_time;
	time_t	now;

	for (std::map<int, Server*>::iterator it = this->server_map_.begin(); it != this->server_map_.end(); ++it) {
		server_last_time = it->second->getLatestServerActivity();
		if (server_last_time > this->latest_server_time_)
			this->latest_server_time_ = server_last_time;
	}

	time(&now);
	if (difftime(now, this->latest_server_time_) >= SERVER_SHUTDOWN_TIME_SEC)
		return true;
	return false;
}

/*! \brief remove client.
 * 
 * First close the client socket and then remove client from client_map_.
 * 
 * @param client_fd client's file descriptor.
 */
void	ServerManager::removeClient( int client_fd ) {

	Logger::log(E_INFO, COLOR_MAGENTA, "Closing socket %d, clearing client data...", client_fd);
	close(client_fd);
	this->client_map_.erase(client_fd);
}

/*! \brief Checks client's server assignment based on the request.
 *		If the request is meant to another server the client's server and its fd are changed in the client attributes.
 *  
 * @param client the client the request was sent from.
 */
void	ServerManager::checkServerAssignmentBasedOnRequest( Client& client ) {

	Server*				current_server = client.getServer();
	const std::string&	request_host = client.getRequest().getRequestHostName();
	int					request_port = client.getRequest().getRequestPort();

	if (current_server->getServerName() != request_host) {
		for (std::map<int, Server*>::const_iterator it = this->server_map_.begin(); it != this->server_map_.end(); it++) {
			if (it->second->getListeningPortInt() == request_port && it->second->getServerName() == request_host) {
				client.setServerAndFd(it->second, it->first);
				Logger::log(E_INFO, COLOR_BRIGHT_YELLOW, "Server for client fd %d changed from server %s => server %s", 
				client.getFd(), current_server->getServerIdforLog().c_str(), it->second->getServerIdforLog().c_str());
			}
		}
	}
}

/*! \brief Receive from from client's socket.
 *
 *	First the latest time is set for the client and its server
 *	Then we recv from the client socket:
 *	If the result was -1 (error) or 0 we return false to disconnect the client connection.
 *	If it is anything else we add the received bytes to the client's request and we also check
 *	which server was the request meant to (if there are multiple servers using the same port, that is).
 * 
 * @param client_fd the file descriptor of the client.
 * @return true if we don't want to close the connection.
 * @return false if we want to close the connection.
 */
bool	ServerManager::receiveFromClient( int client_fd ) {
	
	char		client_msg[500000];
	Client*		client = &this->client_map_[client_fd];
	Server*		server = client->getServer();
	Request&	request = client->getRequest();

	client->setLatestTimeForClientAndServer();
	memset(client_msg, 0, 500000);
	int bytes_received = recv(client_fd, &client_msg, 500000 - 1, 0);

	std::cout << "bytes received: " << bytes_received << std::endl;
	std::cout << "messge:  \n" << client_msg << std::endl;

	if (bytes_received == -1) {
		Logger::log(E_ERROR, COLOR_RED, "recv error from socket %d to server %s, disconnecting client", client_fd, server->getServerIdforLog().c_str());
		return false;
	}
	else if (bytes_received == 0){
		Logger::log(E_INFO, COLOR_MAGENTA, "Client %d has disconnected", client_fd);
		return false;
	} else {
		client_msg[bytes_received] = '\0';
		char* msg_ptr = client_msg;
		client->addToRequest(msg_ptr, bytes_received);
		//check for correct server
		checkServerAssignmentBasedOnRequest(*client);
		Logger::log(E_INFO, COLOR_WHITE, "server %s receives request from socket %d, METHOD=<%s>, URI=<%s>",
			server->getServerName().c_str(), client_fd, request.getRequestLineValue("method").c_str(), request.getRequestLineValue("uri").c_str());
	}
	
	return true;
}

/*! \brief Send response to the Client.
 *
 *	First we check if the client's request has the keep-alive header.
 *	We check if the response_string is empty and if it's not we send the response to the client.
 *	If the send result was -1 (error) we disconnect the client.
 *	Otherwise we check that we sent everything we were supposed to to the client.
 *	If the client's request was complete (as in all of the request has been received and not just a part),
 *	we reset the request and response.
 *	We also check that the response's status code was request timeout and if so, we don't keep the connection alive.
 * 
 *
 *	@param client_fd the file descriptor of the client.
 *	@return true if we want to keep the connection alive.
 *	@return false if we want to close the connection.
 */
bool	ServerManager::sendResponseToClient( int client_fd ) {

	Client*		client = &this->client_map_[client_fd];
	Server*		server = client->getServer();
	Response&	response = client->getResponse();	

	bool	keep_alive = client->getRequest().getKeepAlive();
	const std::string&	response_string = client->getResponseString();

	client->setLatestTimeForClientAndServer();

	if (response_string.empty())
		return keep_alive;
	int	bytes_sent = send(client_fd, response_string.c_str(), response_string.length(), 0);
	// std::cout << "response: \n" << response_string << std::endl;//
	if (bytes_sent == -1) {
		Logger::log(E_ERROR, COLOR_RED, "send error from server %s to socket %d, disconnecting client", server->getServerIdforLog().c_str(), client_fd);
		return false;
	} else {
		if (bytes_sent < static_cast<int>(response_string.length()))
			Logger::log(E_ERROR, COLOR_RED, "incomplete response sent from server %s to socket %d ",
				server->getServerIdforLog().c_str(), client_fd);
		else
			Logger::log(E_INFO, COLOR_WHITE, "server %s sent response to socket %d, STAT=<%d>",
				server->getServerName().c_str(), client_fd, response.getStatusCode());
	}
	//only if complete reset FOR NOW
	if (client->getRequest().getComplete()) {
		client->resetResponse();
		client->resetRequest();
	}
	if (response.getStatusCode() == E_REQUEST_TIMEOUT) {
		return false;
	}
	return keep_alive;
}

/*! \brief Check if client has timed out, and if so, close connection.
*       
*  	@param client_fd the file descriptor of the client.
*/
void	ServerManager::checkIfClientTimeout( int client_fd ) {

	time_t	current_time = time(NULL);
	double	time_since_latest_action = difftime(current_time, client_map_[client_fd].getLatestTime());

	if (time_since_latest_action >= CLIENT_TIMEOUT_SEC) {
		Logger::log(E_INFO, COLOR_BRIGHT_BLUE, "client on socket %d timed out!", client_fd);
		#if POLL_TRUE_SELECT_FALSE
			this->POLL_removeClient(client_fd);
		#else
			this->SELECT_removeClient(client_fd);
		#endif
	}
}


#if	POLL_TRUE_SELECT_FALSE
/********************************************** POLL functions **********************************************************/

	/*! \brief initialize servers for running.
	 *
	 *	Iterate through server_vector_ and call Server::setupServer function which returns the fd of that server.
	 *	Then we make a pollfd for that server and add a pair to the server_map_(fd as key and pointer to server as value). 
	 *
	 * @return true if initialization of servers was successful.
	 * @return false if an error was encountered.
	 */
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

	/*! \brief the main function that handles the management of servers and their clients.
	 * 
	 *	First we set the start time as the latest_server_time_.
	 *	Then we enter an infinity loop that will only break when we reach the server shutdown time.
	 *	in the loop:
	 *	0.5	In the beginning of every loop the printData function will be called if GET_SELECT_POLL_LOOP_FD_INFO is true.
	 *	1.	Then we call poll with all of the fds we want to handle and check if error.
	 *	2.	Then we store the size of the pollfd vector (we need this so that when we remove a client connection we don't iterate over the vector).
	 *	3.	We iterate through the pollfd vector and check if the pollfd has an revent and if so we call handleEvent.
	 *	4.	if the pollfd is a client we check if the client has timed out.
	 *	5.	after iterating the pollfd vector we check if the servers have timed out and if so, break out.
	 * 
	 * @return true if a successful end (ie. the checkServersTimeout returned true)
	 * @return false if there was an error with the poll function (running of servers will be restarted).
	 */
	bool	ServerManager::POLL_runServers( void ) {

		int				poll_result;

		Logger::log(E_INFO, COLOR_GREEN, "runServers() [POLL VERSION] starting...");

		this->latest_server_time_ = time(0);	// get the start time

		while (true) {	//	MAIN LOOP

			#if GET_SELECT_POLL_LOOP_FD_INFO
				this->POLL_printData();	// instead of commenting this out just set the GET_DEBUG_LOG macro to false
			#endif

		
			if ((poll_result = poll(&this->pollfds_[0], this->pollfds_.size(), POLL_TIMEOUT_MILLISEC)) == -1) {
				Logger::log(E_ERROR, COLOR_RED, "POLL ERROR: %s, [WHAT ARE THE CHANCES?!]", strerror(errno));
				this->closeAllSockets();
				return false;
			}

			this->pollfds_size_ = this->pollfds_.size();
			int	i = 0;

			for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); it != this->pollfds_.end() && i < this->pollfds_size_; ++it, ++i) {
				
				if (it->revents & (POLLIN | POLLOUT))
					this->POLL_handleEvent(it);

				if (this->client_map_.count(it->fd))	//	if client, check if timeout
					this->checkIfClientTimeout(it->fd);
			}

			if (this->CheckServersTimeout())	// if there hasn't been any client activity in the server shutdown time, end run
				break;
		}

		Logger::log(E_INFO, COLOR_GREEN, "Server run timeout; servers shutting down... [THE GOOD AND PROPER ENDING]");
		this->closeAllSockets();
		return true;
	}

	/*! \brief Handle a file descriptor event.
	 * 
	 *	if the revent is POLLIN. we check if the fd is a server or client:
	 *	if server, we accept a new connection and create a client
	 *	if client, we receive from the client
	 *	If the revent is POLLOUT, we check if the fd is a client:
	 *	if client, we send a response to the client.
	 *	But if the fd IS NOT a client, we know it is a cgi pipe fd and we
	 *	find out which client it belongs to and then call handleClientCgi.
	 * 
	 * @param it the pollfd vector iterator.
	 */
	void	ServerManager::POLL_handleEvent( std::vector<pollfd>::iterator& it ) {

		int client_fd;

		if (it->revents & POLLIN) {
			if (this->server_map_.count(it->fd)) {
				this->POLL_acceptNewClientConnection(it->fd);
			} else if (this->client_map_.count(it->fd)) {
				this->POLL_receiveFromClient(it->fd);
			}
		} else if (it->revents & POLLOUT) {
			if (this->client_map_.count(it->fd)) {
				this->POLL_sendResponseToClient(it->fd);
			} else {
				client_fd = this->getClientFdByItsCgiPipeFd(it->fd);
				Logger::log(E_DEBUG, COLOR_YELLOW, "pipe fd %d activity spotted, calling handleClientCgi client %d!", it->fd, client_fd);
				this->POLL_handleClientCgi_(client_fd);
			}
		}
	}

	/*! \brief Accept a new client connection.
	 *		Creates a client which is stored into the client map.
	 *	
	 *	First the we accept the client and that is succesful we fcntl that fd to make it non-blocking.
	 *	Then we create the client object which we save into the client_map_ (copies the client object).
	 *	Then we create a pollfd which will represent the client in runServers.
	 *	If there is an error we simply log it and don't add a new client.
	 * 
	 * @param server_fd the file descriptor of the server.
	 */
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

		Client client(server_fd, server);

		this->client_map_[client_fd] = client;
		this->client_map_[client_fd].setLatestTimeForClientAndServer();

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

		if (this->client_cgi_map_.count(client_fd)) {
			this->POLL_removeClientCgiFdsFromPollfds_(client_fd);
		}
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

		if (!this->receiveFromClient(client_fd))
			this->POLL_removeClient(client_fd);
		else {
			Request& request = client.getRequest();
			client.getResponse().createResponsePhase1(&request);

			if (request.getComplete() && request.getCgiFlag() && client.getResponse().getStatusCode() < 400) {
				Logger::log(E_DEBUG, COLOR_BRIGHT_MAGENTA, "valid cgi request, going to startCgiResponse");	// remove later, trying to debug heap use after free error!
				if ((client.startCgiResponse()) == true) {
					CgiHandler* client_cgi = client.getCgiHandler();
					this->addClientCgiFdsToCgiMap_(client_fd, client_cgi->getPipeIn()[E_PIPE_END_WRITE], client_cgi->getPipeOut()[E_PIPE_END_READ]);
					this->POLL_addClientCgiFdsToPollfds_(client_cgi->getPipeIn()[E_PIPE_END_WRITE], client_cgi->getPipeOut()[E_PIPE_END_READ]);
					Logger::log(E_DEBUG, COLOR_BRIGHT_MAGENTA, "cgi pipe added to servermanager pollfds");	// remove later, trying to debug heap use after free error!
				}

				return;
			}
			this->POLL_switchClientToPollout(client_fd);
		}
	}


	void	ServerManager::POLL_sendResponseToClient( int client_fd ) {

		if (this->sendResponseToClient(client_fd)) { // keep alive 
			this->POLL_switchClientToPollin(client_fd);
		} else { // don't keep alive
			this->POLL_removeClient(client_fd);
		}
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

#else
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

		Logger::log(E_INFO, COLOR_GREEN, "runServers() [SELECT VERSION] starting...");

		this->latest_server_time_ = time(0);	// get the start time

		while (true) {	//	MAIN LOOP

			this->SELECT_runServersLoopStart(select_timeout, read_fd_set_copy, write_fd_set_copy);	//ready everything for select

			#if GET_SELECT_POLL_LOOP_FD_INFO
				this->SELECT_printSetData();	// instead of commenting this out just set the GET_DEBUG_LOG macro to false
			#endif
		
			if ((select_result = select(this->biggest_fd_ + 1, &read_fd_set_copy, &write_fd_set_copy, NULL, &select_timeout)) == -1) {
				Logger::log(E_ERROR, COLOR_RED, "SELECT ERROR: %s, [WHAT ARE THE CHANCES?!]", strerror(errno));
				this->closeAllSockets();
				return false;
			}
		
			for (int fd = 0; fd <= this->biggest_fd_; ++fd) {
				
				if (FD_ISSET(fd, &read_fd_set_copy) || FD_ISSET(fd, &write_fd_set_copy)) {
					this->SELECT_handleEvent(fd, read_fd_set_copy, write_fd_set_copy);
				}

				if (this->client_map_.count(fd)) {	//	if client, check if timeout
					this->checkIfClientTimeout(fd);
				}
			}
		
			if (this->CheckServersTimeout())	// if the haven't been any client activity in the server shutdown time end run
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

	void	ServerManager::SELECT_handleEvent( int fd, fd_set& read_fd_set_copy, fd_set& write_fd_set_copy ) {

		if (FD_ISSET(fd, &read_fd_set_copy)) {
			if (this->server_map_.count(fd)) {
				this->SELECT_acceptNewClientConnection(fd);	// new client connection
			} else if (this->client_map_.count(fd)) {
				this->SELECT_receiveFromClient(fd);	// client request or disconnection
			}
		} else if (FD_ISSET(fd, &write_fd_set_copy)) {
			if (this->client_map_.count(fd)) {	// send a response to client
				this->SELECT_sendResponseToClient(fd);
			} else {	//cgi pipe activity
				Logger::log(E_DEBUG, COLOR_YELLOW, "pipe fd %d activity spotted, calling handleClientCgi client %d!", fd, this->getClientFdByItsCgiPipeFd(fd));
				this->SELECT_handleClientCgi_(this->getClientFdByItsCgiPipeFd(fd));
			}
		}
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

		Client client(server_fd, server);

		this->client_map_[client_fd] = client; 
		this->client_map_[client_fd].setLatestTimeForClientAndServer();

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

		if (this->client_cgi_map_.count(client_fd)) {
			this->SELECT_removeClientCgiFdsFromSets_(client_fd);
		}
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

		if (!this->receiveFromClient(client_fd))
			this->SELECT_removeClient(client_fd);
		else {
			Request& request = client.getRequest();
			client.getResponse().createResponsePhase1(&request);

			if (request.getComplete() && request.getCgiFlag() && client.getResponse().getStatusCode() < 400) {
				if ((client.startCgiResponse()) == true) {
					CgiHandler* client_cgi = client.getCgiHandler();
					this->addClientCgiFdsToCgiMap_(client_fd, client_cgi->getPipeIn()[1], client_cgi->getPipeOut()[0]);
					this->SELECT_addClientCgiFdsToSets_(client_cgi->getPipeIn()[1], client_cgi->getPipeOut()[0]);
				}
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
#endif


/* CLASS PRIVATE METHODS */

void	ServerManager::addClientCgiFdsToCgiMap_( int client_fd, int pipe_in, int pipe_out ) {

	if (!this->client_cgi_map_.count(client_fd)) {

		std::vector<int> pipe_vector;
		pipe_vector.push_back(pipe_out);
		pipe_vector.push_back(pipe_in);

		this->client_cgi_map_[client_fd] = pipe_vector;
	} else
		Logger::log(E_ERROR, COLOR_RED, "addClientCgiToCgiMap_; client %d is already in the map (THIS SHOULDN'T HAPPEN)", client_fd); // this is triggered
		// handle error somehow
}

#if POLL_TRUE_SELECT_FALSE
/********************************************** POLL functions **********************************************************/

	void	ServerManager::POLL_addClientCgiFdsToPollfds_( int pipe_in, int pipe_out ) {

		pollfd pollfd_in = {pipe_in, POLLIN, 0};
		pollfd pollfd_out = {pipe_out, POLLOUT, 0};

		this->pollfds_.push_back(pollfd_in);
		this->pollfds_.push_back(pollfd_out);
		// this->pollfds_size_ += 2;
	}

	void	ServerManager::POLL_removeClientCgiFdsFromPollfds_( int client_fd ) {

		for (std::map<int, std::vector<int> >::iterator it = this->client_cgi_map_.begin(); it != this->client_cgi_map_.end(); ++it) {
			if (it->first == client_fd) {
				this->POLL_removeFdFromPollfds(it->second[0]);
				this->POLL_removeFdFromPollfds(it->second[1]);
				break;
			}
		}
	}

	void	ServerManager::POLL_handleClientCgi_( int client_fd ) {

		Client&	client = this->client_map_[client_fd];

		client.finishCgiResponse();
		this->POLL_removeClientCgiFdsFromPollfds_(client_fd);
		this->client_cgi_map_.erase(client_fd);
		this->POLL_switchClientToPollout(client_fd);
		return;
	}

#else
/********************************************** SELECT functions **********************************************************/

	void	ServerManager::SELECT_addClientCgiFdsToSets_( int pipe_in, int pipe_out ) {

		FD_SET(pipe_in, &this->write_fd_set_);
		FD_SET(pipe_out, &this->read_fd_set_);
	}

	void	ServerManager::SELECT_removeClientCgiFdsFromSets_( int client_fd ) {

		for (std::map<int, std::vector<int> >::iterator it = this->client_cgi_map_.begin(); it != this->client_cgi_map_.end(); ++it) {
			if (it->first == client_fd) {
				this->SELECT_removeFdFromSets(it->second[0]);
				this->SELECT_removeFdFromSets(it->second[1]);
				break;
			}
		}
	}

	void	ServerManager::SELECT_handleClientCgi_( int client_fd ) {
		
		Client&	client = this->client_map_[client_fd];

		if (client.getRequest().getCgiFlag()) {
			client.finishCgiResponse();
			this->SELECT_removeClientCgiFdsFromSets_(client_fd);
			this->client_cgi_map_.erase(client_fd);
			this->SELECT_switchClientToWriteSet(client_fd);
			return;
		} else {
			Logger::log(E_ERROR, COLOR_RED, "SELECT_handleClientCgi_; client %d cgi flag was false (THIS SHOULDN'T HAPPEN)", client_fd);
			// handle this error somehow; you can also remove the first if-statement
			// if (client.getRequest().getCgiFlag())
			// because of course it is a cgi!
		}
	}
#endif