#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "ResponseCodes.hpp"
# include "utility.hpp"
# include <ctime>
# include <time.h>
# include <ostream>
// # include <multimap>

# include "Server.hpp"
# include "Request.hpp"
# include "Logger.hpp"

# ifndef CRLF
#  define CRLF "\r\n"
# endif

/*! \brief Holds relevant MIME type information for a given extension.
*       
*
*  Hold MIME type information for a given extension including the mime_type notation
*  and if the type should be sent as binary.
*  
*/
struct s_mime
{
	std::string	extension;
	std::string mime_type;
	bool		binary;
};


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
		Response( const Response& to_copy );
		Response&	operator=( const Response& to_copy );

		/* PRIVATE METHODS AND MEMBERS */
		std::string		response_;
		std::string		body_;
		std::string		response_mime_;
		int				status_code_;
		Server*			server_;
		Request*		request_;
		

		static	std::map<std::string, s_mime> mime_types_;

		void	intializeMimeTypes( void );

		/*HEADER GENERATORS*/
		std::string		timeStampHeader_( void ) const;

		void	getMethod_( void );
		void	headMethod_( void );
		void	deleteMethod_( void );
		void	postMethod_( void );
		bool	methodAllowed_( std::string method );
		void	buildPlainTextBody_( std::string, std::ios_base::openmode mode );
		// void	buildBinaryTextBody_( void );
		bool	uriLocationValid_( std::string uri);

		/*TYPEDEF*/
		typedef	void	(Response::*response_methods_[]) ( void );

	public:
		Response( Server* server );
		~Response( void );

		/* PUBLIC METHODS */

		void			generate( Request* request );
		void			clear( void ); /*reset for next use*/
		const char*		get();
};

#endif
