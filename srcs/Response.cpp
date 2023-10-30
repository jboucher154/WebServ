
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
const char*	Response::get( /*socket to write to?*/ ) {

	std::string response;
	
	response = ResponseCodes::getCodeStatusLine(this->status_code_);
	this->body_ = ResponseCodes::getCodeElementBody(this->status_code_);
	response += this->timeStampHeader() + CRLF;
	response += "Content-Length : ";
	response += int_to_string(this->body_.length());
	response += "\r\n\r\n";
	response += this->body_ + CRLF + CRLF;
	return (response.c_str());
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

//check if logging time stamp can do the same thing
std::string		Response::timeStampHeader( void ) const{

	time_t	now = time(0);
	struct tm tstruct;
	std::string	time_stamp("Date: ");
	char	buffer[100];

	tstruct = *gmtime(&now);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %X GMT", &tstruct);
	time_stamp += buffer;
	return time_stamp;
}