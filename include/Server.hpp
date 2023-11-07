#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <iostream>
# include <string>
# include <sstream>
# include <iterator>
# include <arpa/inet.h>
# include "utility.hpp"

class Server
{
private:
	int					listening_port_; 
	std::string			server_name_;
	std::string			host_;
	std::string			root_;
	struct sockaddr_in	address_;
	int					client_max_body_size_;
	std::string			index_;
	std::string			error_page_;
	std::map<std::string, std::map<std::string, std::vector<std::string> > >	location;
public:
	Server();
	Server( std::string serverName, int port,  std::string host );
	Server( const Server& src );
	Server& operator=( const Server& rhs );
	~Server();

	void						setListeningPort( int port );
	void						setServerName( std::string serverName );
	void						setRoot( std::string root );
	void						setHost( std::string host );
	void						setClientMaxBodySize( int clientMaxBodySize );
	void						setIndex( std::string index );
	void						setErrorPage( std::string errorPage );
	void						setLocation( std::string locationBlockKey, std::string key, std::vector<std::string> values );
	
	// added by ssalmi for server management
	int							setupServer();
	std::string					getServerIdforLog() const;

	int								getListeningPortInt( void ) const;
	std::string						getListeningPortString( void ) const;
	std::string						getServerName( void ) const;
	std::string						getRoot( void ) const;
	std::string						getHost( void ) const;
	in_addr_t						getHostInAddr_t( void ) const;
	int								getClientMaxBodySize( void ) const;
	std::string						getIndex( void ) const;
	std::string						getErrorPage( void ) const;
	struct sockaddr_in				getAddress( void ) const;
	int								getLocationBlockCount( void ) const;
	int								getLocationBlockCount( std::string locationBlockKey ) const;
	std::vector<std::string>		getLocationBlockKeys( void ) const;
	const std::vector<std::string>	getLocationKeys( std::string locationBlockKey ) const;
	const std::vector<std::string>*	getLocationValue( std::string locationBlockKey, std::string key ) const;
	bool							isKeyInLocation( std::string locationBlockKey, std::string key ) const;
	bool							isLocationInServer( std::string locationBlockKey ) const;
};

#endif
