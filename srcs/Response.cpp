
#include "Response.hpp"

/* CONSTRUCTORS */

Response::Response( void )
: response_(""), body(""), status_code_(501) {

	/* default constructor */
}

Response::Response( const Response& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

Response::~Response( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

Response&	Response::operator=( const Response& rhs ) {

	if (this != &rhs) {

		this->response_ = rhs.response_;
		this->status_code_ = rhs.status_code_;
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */

// void	Response::generateResponse( Request& request ) {

// 	/* generate response */
// }

std::string	Response::send( /*socket to write to?*/ ) const {

	return (ResponseCodes::getCombinedStatusLineAndBody(this->status_code_));
	/* send response */
}

void	Response::clear( void ) { 	/* reset for next use */

	this->response_ = "";
	this->body_ = "";
	this->status_code_ = 501;
}

/* CLASS PRIVATE METHODS */
