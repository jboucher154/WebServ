#ifndef CLIENT_HPP
# define	CLIENT_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "CgiHandler.hpp"

# include <cstring>

/*! \brief forward declaration for cgiHandlerclass */
class CgiHandler;

/*! \brief class contains all relevant information for server client
*
*	@class Client
*	Client class contains the clients file descritpor, address info, the server
*	it connected with, along with the objects to handle the requests and responses
*	for the client.
*	
*/
class	Client {
	private:

		int					fd_; /*!< \brief client's file (socket) descriptor */
		struct sockaddr_in	address_; /*!< \brief client's address information */			
		time_t				latest_time_; /*!< \brief time of client's latest action (used for disconneting and server automatic shutdown) */ 
		Server*				server_; /*!< \brief pointer to the server the client is connected to */
		int					server_fd_; /*!< \brief file (socket) descriptor of the server the client is connected to */ 
		Request				request_; /*!< \brief requst object to parse and handle client requests */
		Response			response_; /*!< \brief response object to generate HTTP responses */
		CgiHandler*			cgi_handler_; /*!< \brief object to run cgi processes */

	public:
	
		Client( void );
		Client( int server_fd, Server* server );
		Client( const Client& other );
		~Client( void );

		Client&	operator=( const Client& rhs );

		bool					startCgiResponse( void );
		void					finishCgiResponse( void );

		/* SETTERS */
		void					setLatestTimeForClientAndServer( void );
		void					setServerAndFd( Server* server, int new_server_fd );

		/* GETTERS */
		int						getFd( void ) const;
		time_t					getLatestTime( void ) const;
		Server*					getServer( void ) const;
		int						getServerFd( void ) const;
		std::string				getClientHost( void ) const;
		struct sockaddr_in&		getAddress( void );
		CgiHandler*				getCgiHandler( void );
		Request&				getRequest( void );
		Response&				getResponse( void );
		const std::string&		getResponseString( void );
		void					addToRequest( char* message, size_t bytes_read );
		void					resetResponse( void );
		void					resetRequest( void );
};

#endif