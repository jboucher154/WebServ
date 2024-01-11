#ifndef	SERVERMANAGER_HPP
# define	SERVERMANAGER_HPP

# include "WebServ.hpp"
# include "Server.hpp"
# include "Client.hpp"

/*! \brief Server Manager class.
 *         This class handles managing the server and client sockets + client cgi pipes.
 *
 *	The ServerManager does what its name implies. It handles stuff like setting up server sockets, creating client sockets and connections
 *	and switching of the client between receiving requests and sending responses, adding possible cgi pipes to the monitored file descriptors,
 *	monitoring that inactive client connections are cut after a certain, and shutting down the program if there is no activity for a certain time.
 */
class	ServerManager {
	private:
		std::vector<Server>&				servers_; /*!< @brief servers_ is a reference to Validator class generated servers passed to ServerManager constructor.*/

		std::map<int, std::vector<int> >	client_cgi_map_; /*!< @brief client_cgi_map_ is a map of client_fd and the vector of in and out pipes accoiciated to each client_fd .*/
		std::vector<struct pollfd>		pollfds_; /*!< @brief pollfds_, a vector of pull fds created from server_sockets.*/
		int								pollfds_size_; /*!< @brief pollfds_size_, is the size of the pull fds.*/
		std::map<int, Server*>			server_map_; /*!< @brief server_map_ is a map of server fd and the server accoiciated to each fd .*/
		std::map<int, Client>			client_map_; /*!< @brief client_map_ is a map of client_fd and the client accoiciated to each client_fd .*/
		time_t							latest_server_time_; /*!< @brief latest_server_time_ is the lastest time any server had any activity. latest_server_time_ is always checked againest SERVER_SHUTDOWN_TIME_SEC to see if webserver should still sit idle or not*/
		
		void	addClientCgiFdsToCgiMap_( int client_fd, int pipe_in, int pipe_out );
		void	POLL_addClientCgiFdsToPollfds_( int pipe_in, int pipe_out );
		void	POLL_removeClientCgiFdsFromPollfds_( int client_fd );
		void	POLL_handleClientCgi_( int client_fd );

	
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
		void	checkServerAssignmentBasedOnRequest( Client& client );

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