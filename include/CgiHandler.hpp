#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "Client.hpp"
# include "WebServ.hpp"

# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <vector>
# include <map>
# include <string>

// forward declaration
class Client;

// macro for size of the buffer used in storeCgiOutput_
# define BUFFER_SIZE 4096

enum	e_pipe_ends {
	E_PIPE_END_READ,
	E_PIPE_END_WRITE
};

/*! \brief CgiHandler class handles the setting up and execution of cgi scripts in the webserver .
 *		Every client has a CgiHandler object which it uses to handle cgi requests.
 *
 *	The handling of a cgi requests proceeds in the following way:
 *	1. if the request was valid, the ServerManager::receiveFromClient will call Client::startCgiResponse
 *	which will initialize the cgi with CgiHandler::initializeCgi.
 *	2. if this was successful, the pipe ends used to communicate with the cgi process will be added
 *	to the fds the ServerManager monitors.
 *	3. the cgi pipe used to read from the cgi process will trigger an event in the ServerManager::runServer function.
 *	4. the ServerManager::handleEvent function will detect that which client this cgi pipe end belongs to and
 *	will call that client's Client::cgiFinishCgiResponse function which in turn calls CgiHandler::cgiFinish.
 *	5. in cgiFinish the cgi script will be executed and its results will be stored.
 *	6. if this was successful the results will be passed forward to the client.
 *
 *	If there are any errors during the cgi handling, the client will be informed of a server error. 
 */
class	CgiHandler {

	private:
		std::map<std::string, std::string>	metavariables_map_;
		std::string			cgi_output_as_string_;
		char**				metavariables_;
		char**				args_;
		char*				path_;
		int					pid_;
		int					pipe_into_cgi_[2];
		int					pipe_from_cgi_[2];


		int		fillMetavariablesMap_( Client& client );
		char**	convertMetavariablesMapToCStringArray_( void );
		int		createCgiArguments_( std::string uri, Client& client );
		int		cgiTimer_( int& status );

		int		setUpCgiPipes_( void );
		int		executeCgi_( const std::string& body_string );
		int		storeCgiOutput_( void );

	public:
		CgiHandler( void );
		CgiHandler( const CgiHandler& to_copy );

		~CgiHandler( void );

		CgiHandler&	operator=( const CgiHandler& to_copy );

		void	ClearCgiHandler( void );
		void	closeCgiPipes( void );
		int		initializeCgi( Client& client );
		int		cgiFinish( Response& response );
		void	clearCgiOutputs( void );

		// getters
		std::vector<std::string, std::string>&	getMetavariablesVector( void ) const;
		std::vector<std::string, std::string>&	getCgiVector( void ) const;
		std::string					getExtension( std::string uri );
		const std::string&			getCgiOutputAsString( void ) const;
		const int*	getPipeIn( void ) const;
		const int*	getPipeOut( void ) const;
};

#endif
