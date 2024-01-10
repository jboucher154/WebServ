#ifndef	SERVERMANAGER_HPP
/*! \brief Server Manager class.
 *         This class handles managing the server and client sockets + client cgi pipes.
 *
 *	The ServerManager does what its name implies. It handles stuff like setting up server sockets, creating client sockets and connections
 *	and switching of the client between receiving requests and sending responses, adding possible cgi pipes to the monitored file descriptors,
 *	monitoring that inactive client connections are cut after a certain, and shutting down the program if there is no activity for a certain time.
 */
# define	SERVERMANAGER_HPP

# include "WebServ.hpp"
# include "Server.hpp"
# include "Client.hpp"

class	ServerManager {
	private:
		std::vector<Server>&				servers_;

		std::map<int, std::vector<int> >	client_cgi_map_;

		#if POLL_TRUE_SELECT_FALSE
			std::vector<struct pollfd>		pollfds_;
			int								pollfds_size_;
		#else
			fd_set							read_fd_set_;
			fd_set							write_fd_set_;
			int								biggest_fd_;
		#endif

		std::map<int, Server*>			server_map_;
		std::map<int, Client>			client_map_;
		time_t							latest_server_time_;

		void	addClientCgiFdsToCgiMap_( int client_fd, int pipe_in, int pipe_out );

		void	POLL_addClientCgiFdsToPollfds_( int pipe_in, int pipe_out );
		void	POLL_removeClientCgiFdsFromPollfds_( int client_fd );
		void	POLL_handleClientCgi_( int client_fd );

		void	SELECT_addClientCgiFdsToSets_( int pipe_in, int pipe_out );
		void	SELECT_removeClientCgiFdsFromSets_( int client_fd );
		void	SELECT_handleClientCgi_( int client_fd );
	
	public:
		ServerManager( std::vector<Server>& server_vector );
		ServerManager( const ServerManager& other );
		~ServerManager( void );

		ServerManager&	operator=( const ServerManager& rhs );

		void	closeServerSockets( void );
		void	closeAllClientConnections( void );
		void	closeAllSockets( void );
		bool	CheckServersTimeout( void );
		void	removeClient( int client_fd );
		bool	receiveFromClient( int client_fd );
		bool	sendResponseToClient( int client_fd );
		int		getClientFdByItsCgiPipeFd( int pipe_fd );
		void	checkIfClientTimeout( int client_fd );
		void	checkServerAssignmentBasedOnRequest( Client& client );//


		bool	SELECT_initializeServers( void );
		void	SELECT_initializeFdSets( void );
		int		SELECT_getBiggestFd( int max_fd_size );
		bool	SELECT_runServers( void );
		void	SELECT_runServersLoopStart( timeval& select_timeout, fd_set& read_fd_set_copy, fd_set& write_fd_set_copy );
		void	SELECT_handleEvent( int fd, fd_set& read_fd_set_copy, fd_set& write_fd_set_copy );
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
		void	POLL_handleEvent( std::vector<pollfd>::iterator& it );
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