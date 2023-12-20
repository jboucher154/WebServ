#ifndef	SERVERMANAGER_HPP
# define	SERVERMANAGER_HPP

# include "WebServ.hpp"
# include "Server.hpp"
# include "Client.hpp"

// for timing out select()
# define SELECT_TIMEOUT_SEC 2
# define SELECT_TIMEOUT_USEC 0

// for timing out poll()
# define POLL_TIMEOUT_MILLISEC 2 * 1000

// time the servers will run for without any client action (in seconds)
# define	SERVER_SHUTDOWN_TIME_SEC 7 * 60


// time a client connection is kept open from last client action (in seconds)
# define	CLIENT_TIMEOUT_SEC 1 * 60

/*! \brief Server Manager class.
*         This class handles managing the server and client sockets.
*
*  Detailed description starts here.
*	BE AWARE: I have the poll() and select() variables/functions here (labeled clearly), the one we don't go with will be removed
*/

class	ServerManager {
	private:
		std::vector<Server>					servers_;				// vector of all servers

		std::map<int, std::vector<int> >	client_cgi_map_;		// map of client fds and their pipe fds (for cgi)

		#if POLL_TRUE_SELECT_FALSE
			std::vector<struct pollfd>		pollfds_;				// vector of all server, client and cgi pipe pollfds (POLL)
			int								pollfds_size_;			// this is needed so that you don't iterate over a vector after you erase a pollfd (POLL)
		#else
			fd_set							read_fd_set_;			// set of file descriptors ready for read (SELECT)
			fd_set							write_fd_set_;			// set of file descriptors ready for write (SELECT)
			int								biggest_fd_;			// biggest fd value (SELECT)
		#endif

		std::map<int, Server*>			server_map_;			// map of server socket descriptors and a pointers to servers objects
		std::map<int, Client>			client_map_;			// map of client socket descriptors and Client objects
		time_t							last_client_time_;		// for automatic shutdown

		void	addClientCgiFdsToCgiMap_( int client_fd, int pipe_in, int pipe_out );

		void	POLL_addClientCgiFdsToPollfds_( int pipe_in, int pipe_out );
		void	POLL_removeClientCgiFdsFromPollfds_( int client_fd );
		void	POLL_handleClientCgi_( int client_fd );

		void	SELECT_addClientCgiFdsToSets_( int pipe_in, int pipe_out );
		void	SELECT_removeClientCgiFdsFromSets_( int client_fd );
		void	SELECT_handleClientCgi_( int client_fd );

	public:
		ServerManager( void );
		ServerManager( std::vector<Server>& server_vector );
		ServerManager( const ServerManager& other );
		~ServerManager( void );

		ServerManager&	operator=( const ServerManager& rhs );

		void	closeServerSockets( void );
		void	closeAllClientConnections( void );
		void	closeAllSockets( void );
		bool	checkLastClientTime( void );
		void	removeClient( int client_fd );
		bool	receiveFromClient( int client_fd );
		bool	sendResponseToClient( int client_fd );
		int		getClientFdByItsCgiPipeFd( int pipe_fd );
		void	checkIfClientTimeout( int client_fd );


		bool	SELECT_initializeServers( void );
		void	SELECT_initializeFdSets( void );
		int		SELECT_getBiggestFd( int max_fd_size );
		bool	SELECT_runServers( void );
		void	SELECT_runServersLoopStart( timeval& select_timeout, fd_set& read_fd_set_copy, fd_set& write_fd_set_copy );
		void	SELECT_acceptNewClientConnection( int server_fd );
		void	SELECT_removeFdFromSets( int fd );
		void	SELECT_removeClient( int client_fd );
		void	SELECT_switchClientToReadSet( int client_fd );
		void	SELECT_switchClientToWriteSet( int client_fd );
		void	SELECT_receiveFromClient( int client_fd );
		void	SELECT_sendResponseToClient( int client_fd );
		void	SELECT_printSetData( void );
		void	SELECT_addFdsToFdSets( Client& client );
		void	SELECT_removeFdsFromFdSets( Client& client );


		bool	POLL_initializeServers( void );
		bool	POLL_runServers( void );
		void	POLL_acceptNewClientConnection( int server_fd );
		void	POLL_removeFdFromPollfds( int fd );
		void	POLL_removeClient( int client_fd );
		void	POLL_switchClientToPollin( int client_fd );
		void	POLL_switchClientToPollout( int client_fd );
		void	POLL_receiveFromClient( int client_fd );
		void	POLL_sendResponseToClient( int client_fd );
		void	POLL_printData( void );
		void	POLL_addFdsToPollfds( Client& client );
		void	POLL_removeFdsFromPollfds( Client& client );
};

#endif