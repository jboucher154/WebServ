#ifndef	SERVERMANAGER_HPP
#define	SERVERMANAGER_HPP

#include "Sockets.hpp"
#include "Client.hpp"

// for timing out of select() or poll()
#define TIMEOUT_SEC 2
#define	TIMEOUT_USEC 0

// time the servers will run for without any client action (in seconds)
#define	SERVER_SHUTDOWN_TIME_SEC 2 * 60

/*! \brief Server Manager class.
*         This class handles managing the server and client sockets.
*
*  Detailed description starts here.
*	BE AWARE: I have the poll() and select() variables/functions here (labeled clearly), the one we don't go with will be removed
*/

class	ServerManager {
	private:
		std::vector<Server>			servers_;		// vector of all servers

		fd_set						master_fd_set_;		// set of all server and client file descriptors (SELECT)
		fd_set						read_fd_set_;		// set of file descriptors ready for read (SELECT)
		fd_set						write_fd_set_;		// set of file descriptors ready for write (SELECT)
		int							biggest_fd_;		// biggest fd value (needed for SELECT)

		std::vector<struct pollfd>	pollfds_;		// vector of all server and client pollfds (POLL)

		std::map<int, Server*>		server_map_;	// map of server socket descriptors and a pointers to servers objects
		std::map<int, Client>		client_map_;	// map of client socket descriptors and Client objects
		time_t						last_client_time_;	// for automatic shutdown

	public:
		ServerManager( void );
		ServerManager( const ServerManager& other );
		~ServerManager( void );

		ServerManager&	operator=( const ServerManager& rhs );

		// int		getServerListenerSocket( Server& server );
		void	closeServerSockets( void );
		void	closeClientSockets( void );
		void	closeAllSockets( void );
		bool	checkLastClientTime( void );

		bool	SELECT_initializeServers( void );
		void	SELECT_initializeFdSets( void );
		int		SELECT_runServers( std::vector<Server>& server_vector );
		void	SELECT_acceptNewClientConnection( int server_fd );

		bool	POLL_initializeServers( void );

};

int	getBiggestFdOfSet( int max_fd_size, fd_set* set );

#endif