#ifndef CLIENT_HPP
#define	CLIENT_HPP

// my old server-functions
// #include "Sockets.hpp"
#include <Server.hpp>

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
		// request_
		// response_
		
	public:
		Client( void );
		Client( int server_fd, Server* server );
		// Client( const Client& other );
		~Client( void );

		// Client&	operator=( const Client& rhs );

		// setters
		// int					setupClient( void );
		void				setLatestTime( void );

		// getters
		int					getFd( void ) const;
		// struct sockaddr_in&	getAddress( void ) const;
		time_t				getLatestTime( void ) const;
		Server*				getServer( void ) const;
		int					getServerFd( void ) const;
		std::string			getFullClientIdForLog( void ) const;
};

#endif