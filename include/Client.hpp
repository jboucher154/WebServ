#ifndef CLIENT_HPP
# define	CLIENT_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "CgiHandler.hpp"

// forward declaration, because of this have to use pointer inside Client class
class CgiHandler;

/*! \brief Brief description.
*         Brief description continued.
*
*  Detailed description starts here.
*/
class	Client {
	private:
		int					fd_;					// client's file (socket) descriptor
		struct sockaddr_in	address_;				// client's address					
		time_t				latest_time_;			// time of client's latest action (used for disconneting and server automatic shutdown)
		Server*				server_;				// pointer to the server the client is connected to
		int					server_fd_;				// file (socket) descriptor of the server the client is connected to

		Request				request_;
		Response			response_;
		CgiHandler*			cgi_handler_;

	public:
		Client( void );
		Client( int server_fd, Server* server );
		Client( const Client& other );
		~Client( void );

		Client&	operator=( const Client& rhs );

		bool	SELECT_startCgiResponse( void );
		void	SELECT_finishCgiResponse( void );

		bool	POLL_startCgiResponse( void );
		void	POLL_finishCgiResponse( void );

		// setters
		void				setLatestTime( void );

		// getters
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
		void					addToRequest( std::string message );
		void					resetResponse( void );
		void					resetRequest( void );
};

#endif