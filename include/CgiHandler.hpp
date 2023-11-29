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

enum	e_cgi_results {
	E_CGI_OK,
	E_CGI_SERVERERROR,
	E_CGI_NOTFOUND,
	E_CGI_NOPERMISSION,
	E_CGI_UNKNOWNMETHOD
};

class	CgiHandler {

	private:
		std::map<std::string, std::string>	metavariables_map_;
		std::map<std::string, std::string>	cgi_map_;

		std::string	cgi_output_;
		char**		metavariables_;
		char**		args_;
		char*		path_;
		bool		piping_successful_;
		bool		forking_successful_;
		int			pid_;
		int			pipe_in_[2];
		int			pipe_out_[2];

		// these are needed for POLL/SELECT
		std::vector<pollfd>		pollfds_;
		fd_set					read_fd_set_;
		fd_set					write_fd_set_;

		void	SELECT_cgiRemoveFdFromSets_( int fd );
		void	SELECT_cgiRemoveCgiPipeEndsFromSets_( int pipe_in, int pipe_out );
		void	SELECT_cgiAddFdToReadSet_( int fd );
		void	SELECT_cgiAddFdToWriteSet_( int fd );

		void	POLL_cgiRemoveFdFromPollfds_( int fd );
		void	POLL_cgiRemoveCgiPipeEndsFromPollfds_( int pipe_in, int pipe_out );
		void	POLL_cgiAddFdtoPollfds_( int fd, int mode );
		// end of POLL/SELECT attributes and methods

		int		fillMetavariablesMap_( Client& client );
		char**	convertMetavariablesMapToCStringArray_( void );
		int		createCgiArguments_( std::string uri );
		void	cgiTimer_( int& status );

		int		SELECT_setUpCgiPipes_( void );
		int		SELECT_executeCgi_( std::vector<std::string>::iterator it_start, std::vector<std::string>::iterator it_end );
		int		SELECT_storeCgiOutput_( void );

		int		POLL_setUpCgiPipes_( void );
		int		POLL_executeCgi_( std::vector<std::string>::iterator it_start, std::vector<std::string>::iterator it_end );
		int		POLL_storeCgiOutput_( void );

	public:
		CgiHandler( void );
		CgiHandler( const CgiHandler& to_copy );

		~CgiHandler( void );

		CgiHandler&	operator=( const CgiHandler& to_copy );

		void	ClearCgiHandler( void );
		void	closeCgiPipes( void );
		void	clearCgiHandlerPollfdsAndSets( void );

		int		SELECT_initializeCgi( Client& client );
		int		SELECT_cgiFinish( Response& response );


		int		POLL_initializeCgi( Client& client );
		int		POLL_cgiFinish( Response& response );

		// setters
		void	setMetaVariables( const char** metavariables );
		void	setPipingSuccessful( bool piping_result );
		void	setForkingSuccessful( bool forking_result );

		// getters
		std::vector<std::string, std::string>&	getMetavariablesVector( void ) const;
		std::vector<std::string, std::string>&	getCgiVector( void ) const;
	
		std::string		getExtension( std::string uri );
		const std::string&	getCgiOutput( void ) const;

		std::vector<pollfd>&	getCgiPollfds( void );
		fd_set&					getCgiReadSet( void );
		fd_set&					getCgiwriteSet( void );

		char**	getMetaVariables( void ) const;
		char**	getArgs( void ) const;
		bool	getPipingSuccessful( void ) const;
		bool	getForkingSuccessful( void ) const;
		int*	getPipeIn( void ) const;
		int*	getPipeOut( void ) const;
};

#endif
