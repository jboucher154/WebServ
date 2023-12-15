#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "Client.hpp"

# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <vector>
# include <map>
# include <string>

// forward declaration
class Client;

// macro for cgi timeout
# define CGI_TIMEOUT 5

// macro for buffer for cgi output reading
# define CGI_OUTPUT_BUFFER 102400

enum	e_cgi_results {
	E_CGI_OK,
	E_CGI_SERVERERROR,
	E_CGI_NOTFOUND,
	E_CGI_NOPERMISSION,
	E_CGI_UNKNOWNMETHOD
};

enum	e_pipe_ends {
	E_PIPE_END_READ,
	E_PIPE_END_WRITE
};

class	CgiHandler {

	private:
		std::map<std::string, std::string>	metavariables_map_;
		// std::map<std::string, std::string>	cgi_map_;

		std::vector<char>	cgi_output_;
		std::string			cgi_output_as_string_;
		char**				metavariables_;
		char**				args_;
		char*				path_;
		bool				piping_successful_;
		bool				forking_successful_;
		int					pid_;
		int					pipe_into_cgi_[2];
		int					pipe_from_cgi_[2];


		int		fillMetavariablesMap_( Client& client );
		char**	convertMetavariablesMapToCStringArray_( void );
		int		createCgiArguments_( std::string uri, Client& client );
		void	cgiTimer_( int& status );

		int		setUpCgiPipes_( void );
		int		executeCgi_( std::vector<std::string>::iterator it_start, std::vector<std::string>::iterator it_end );
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
		const std::vector<char>&	getCgiOutput( void ) const;
		const std::string&			getCgiOutputAsString_( void ) const;

		// char**		getMetaVariables( void ) const;
		char**		getArgs( void ) const;
		bool		getPipingSuccessful( void ) const;
		bool		getForkingSuccessful( void ) const;
		const int*	getPipeIn( void ) const;
		const int*	getPipeOut( void ) const;
};

#endif
