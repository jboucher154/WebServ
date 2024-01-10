#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <iostream>
# include <string>
# include <sstream>
# include <iterator>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>

# include "utility.hpp"
# include "Logger.hpp"
# include "Color.hpp"

// Macro used for listen()
# define LISTEN_BACKLOG	20

/*! \brief stores server information from config and server setup
*
*	@class Server 
*	
*	The server class is intialized by the Validator class and contains all 
*	information for one server section from the config file.
*
*/
class Server {

	private:

		int										listening_port_; /*!< \brief port server is listening to */
		std::string								server_name_; /*!< \brief string of server name from config file */
		std::string								host_; /*!< \brief string of ip address that server is bound to */
		std::string								root_; /*!< \brief relative path from server root to the root of the website to be served */
		struct sockaddr_in						address_; /*!< \brief struct of ip address information for server, intialized in Validator */
		double									client_max_body_size_; /*!< \brief maximum request body size the server should accept */
		std::string								upload_store_; /*!< \brief path to the directory the server uses for temporary file storage  */
		std::map<std::string, std::string>		error_pages; /*!< \brief map of error page ids and paths for custom error pages listed in config */
		std::map<std::string, std::map<std::string, std::vector<std::string> > >	location_; /*!< \brief map of locations from config, each with a map of their keys and values (in a vector) */                                                


	public:

		/*TYPEDEFS*/
		typedef std::map<std::string, std::vector<std::string> >					map_of_str_vec_of_str;
		typedef std::map<std::string, std::vector<std::string> >::const_iterator	const_it_for_map_of_str_vec_of_str;
		typedef std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator const_it_for_map_of_str_map_of_str_vec_of_str;

		Server();
		Server( const Server& src );
		Server& operator=( const Server& rhs );
		~Server();

		void							setListeningPort( int port );
		void							setServerName( std::string serverName );
		void							setRoot( std::string root );
		void							setHost( std::string host );
		void							setClientMaxBodySize( double clientMaxBodySize );
		void							setErrorPage( std::string error_code, std::string errorPage );
		void							setKeyValueInLocation( std::string locationBlockKey, std::string key, std::vector<std::string> values );
		void							setLocation( map_of_str_vec_of_str	innerBlock, std::string key );
		int								setupServer();
		bool							setUploadStore( std::string upload_dir );
		std::string						getServerIdforLog() const;
		int								getListeningPortInt( void ) const;
		std::string						getListeningPortString( void ) const;
		std::string						getServerName( void ) const;
		std::string						getRoot( void ) const;
		std::string						getHost( void ) const;
		in_addr_t						getHostInAddr_t( void ) const;
		double							getClientMaxBodySize( void ) const;
		std::string						getErrorPage( std::string error_code ) const;
		bool							isErrorPage( std::string error_code ) const;
		const std::string&				getUploadStore( void ) const;
		struct sockaddr_in				getAddress( void ) const;
		int								getLocationBlockCount( void ) const;
		int								getLocationBlockCount( std::string locationBlockKey ) const;
		std::vector<std::string>		getLocationBlockKeys( void ) const;
		const std::vector<std::string>	getLocationKeys( std::string locationBlockKey ) const;
		const std::vector<std::string>*	getLocationValue( std::string locationBlockKey, std::string key ) const;
		std::string						getCgiExecutor( std::string extension ) const;
		bool							isKeyInLocation( std::string locationBlockKey, std::string key ) const;
		bool							isLocationInServer( std::string locationBlockKey ) const;
		bool							isValueListedForKey( std::string locationBlockKey, std::string key, std::string value ) const;
		bool							isExtensionOnCgiList( std::string extension ) const;
		bool							isScriptOnCgiList( std::string script ) const;

};

#endif
