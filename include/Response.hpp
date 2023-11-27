#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "ResponseCodes.hpp"
# include <ctime>
# include <time.h>
# include <ostream>
# include <map>
#include <unistd.h>

# include <vector>

# include "utility.hpp"
# include "ServerManager.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "Logger.hpp"
# include "CgiHandler.hpp"

# ifndef CRLF
#  define CRLF "\r\n"
# endif


/*! \brief Class for handling HTTP responses.
*       
*
*  Response class handle HTTP responses. It is responsible for generating the response
*  given a Request object.
*  
*  Public Methods:
*  - generate: generate the response given a Request object
*  - send: send the response to the client
*  - clear: reset the response for next use
*/
class	Response {

	private:
		Response( void );

		/* PRIVATE METHODS AND MEMBERS */
		std::string			response_;
		std::string			body_;
		std::vector<char>	binary_data_;
		std::string			response_mime_;
		std::string			resource_path_; //path for opening/ manipulating etc
		std::string			resource_location_; // location for looking up in server
		int					status_code_;
		Server*				server_;
		Request*			request_;
		std::string			query_string_;
		std::vector<std::string> file_data_;
		//maybe add map of headers, create them as I go?
		CgiHandler			cgi_handler_;	// added by ssalmi
		
		void	intializeMimeTypes( void );

		/*HEADER GENERATORS*/
		std::string&	addHeaders_( std::string& response) const;
		std::string		timeStampHeader_( void ) const;
		std::string		contentTypeHeader_( void ) const;
		std::string 	contentLengthHeader_( void ) const;
		std::string		contentLocationHeader_( void ) const;

		void	getMethod_( void );
		void	headMethod_( void );
		void	deleteMethod_( void );
		void	postMethod_( void );
		bool	methodAllowed_( std::string method );
		void	buildBody_( std::string& path, std::ios_base::openmode mode );
		int		setResourceLocationAndName( std::string uri );
		void	setMimeType( void );

		std::vector<std::string>	getAcceptedFormats( void );

		/*TYPEDEF*/
		typedef	void	(Response::*response_methods_[]) ( void );

	public:
		static	std::map<std::string, std::string> mime_types_;
		Response( const Response& to_copy );
		Response( Server* server );
		~Response( void );

		Response&	operator=( const Response& to_copy );

		/* PUBLIC METHODS */

		void			generate( Request* request ); // call in client ? 
		void			clear( void ); /*reset for next use*/
		std::string&		get();
};

#endif



/*
- verify path to cgi script before handing over to cgi
- check access/ permissions to the resource

*/ 