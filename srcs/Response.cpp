
#include "Response.hpp"

/* CONSTRUCTORS */

/*! \brief Default constructor intializes empty response and body. Default status code set to 501 for MVP.
*       
*
*  Default constructor intializes empty response and body. Default status code set to 501 for MVP.
*  More details to be filled as project progresses.
*/
Response::Response( void )
: response_(""), body_(""), status_code_(501) {

	/* default constructor */
}

/*! \brief Copy constructor calls assignment operator.
*       
*
*  More details to be filled as project progresses.
*  
*/
Response::Response( const Response& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief Default destructor has not special actions.
*       
*
*  More details to be filled as project progresses.
*  
*/
Response::~Response( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

/*! \brief Assignment operator copies all members from rhs to this.
*       
*
*  More details to be filled as project progresses.
*  
*/
Response&	Response::operator=( const Response& rhs ) {

	if (this != &rhs) {

		this->response_ = rhs.response_;
		this->status_code_ = rhs.status_code_;
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */


/*! \brief generate method creates the response given a Request object.
*       
*
*  Not yet implemented.
*  
*/
// void	Response::generate( Request& request ) {

// 	/* generate response */
// }

/*! \brief get method returns the response as a c string.
*       
*
*  Currently returns response from ResponseCodes class based on current status_code_.
*  More details to be filled as project progresses.
*  
*/
const char*	Response::get( /*socket to write to?*/ ) const {

	// return (ResponseCodes::getCombinedStatusLineAndBody(501).c_str());//default response for now
	// return (ResponseCodes::getCombinedStatusLineAndBody(this->status_code_).c_str());
	return (ResponseCodes::getCodeStatusLine(this->status_code_).c_str());
}

/*! \brief clear method resets the response for next use
*       
*
*  More details to be filled as project progresses.
*  
*/
void	Response::clear( void ) { 	/* reset for next use */

	this->response_ = "";
	this->body_ = "";
	this->status_code_ = 0;
}

/* CLASS PRIVATE METHODS */
