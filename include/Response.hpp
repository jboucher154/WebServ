#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "ResponseCodes.hpp"
# include "utility.hpp"
#include <ctime>
#include <time.h>

# ifndef CRLF
#  define CRLF "\r\n"
# endif

// class Request; //forward declaration for now


/*! \brief Class for handling HTTP responses.
*       
*
*  Response class handle HTTP responses. It is responsible for generating the response
*  given a Request object.
*  
*  Methods:
*  - generate: generate the response given a Request object
*  - send: send the response to the client
*  - clear: reset the response for next use
*/
class	Response {

	private:
		Response( void );
		/* PRIVATE METHODS AND MEMBERS */
		std::string		response_;
		std::string		body_;
		int				status_code_;
		// int				http_version_;
		// int				content_length_;
		std::string		timeStampHeader( void ) const;

	public:
		Response( const Response& to_copy );

		~Response( void );

		Response&	operator=( const Response& to_copy );

		/* PUBLIC METHODS */

		// void			generate( Request& request );
		const char*		get( /*socket to write to?*/ );
		void			clear( void ); /*reset for next use*/
};

#endif
