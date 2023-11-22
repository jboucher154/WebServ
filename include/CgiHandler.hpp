#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "Server.hpp"
# include "Client.hpp"
# include "Request.hpp"

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

		char**	metavariables_;
		char**	args_;
		char*	path_;
		bool	piping_successful_;
		bool	forking_successful_;
		int		pipe_in_[2];
		int		pipe_out_[2];
		int		pid_;

		int		fillMetavariablesMap_( Client& client );
		char**	convertMetavariablesMapToCStringArray_( void );
		int		createCgiArguments_( Request& request );

	public:
		CgiHandler( void );
		CgiHandler( const CgiHandler& to_copy );

		~CgiHandler( void );

		CgiHandler&	operator=( const CgiHandler& to_copy );

		/* PUBLIC METHODS */
		void	ClearCgiHandler( void );

		int		SELECT_initializeCgi( Client& client, ServerManager& server_manager );


		int		POLL_initializeCgi( Client& client, ServerManager& server_manager );


		// setters
		void	setMetaVariables( const char** metavariables );
		void	setArgs( const char** args );
		void	setPipingSuccessful( bool piping_result );
		void	setForkingSuccessful( bool forking_result );

		// getters
		std::vector<std::string, std::string>&	getMetavariablesVector( void ) const;
		std::vector<std::string, std::string>&	getCgiVector( void ) const;
		char**	getMetaVariables( void ) const;
		char**	getArgs( void ) const;
		bool	getPipingSuccessful( void ) const;
		bool	getForkingSuccessful( void ) const;
		int*	getPipeIn( void ) const;
		int*	getPipeOut( void ) const;
};

#endif
