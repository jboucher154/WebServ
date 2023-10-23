#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "ResponseCodes.hpp"

class Request;

class	Response {

	private:
		/* PRIVATE METHODS AND MEMBERS */
		std::string		response_;
		std::string		body_;
		int				status_code_;
		// int				http_version_;
		// int				content_length_;

	public:
		Response( void );
		Response( const Response& to_copy );

		~Response( void );

		Response&	operator=( const Response& to_copy );

		/* PUBLIC METHODS */

		// void			generateResponse( Request& request );
		std::string		send( /*socket to write to?*/ ) const;
		void			clear( void ); /*reset for next use*/
};

#endif
