
#include "ResponseCodes.hpp"

std::map<int, std::string> ResponseCodes::codes_;

/* CONSTRUCTORS */

ResponseCodes::ResponseCodes( void ) { /* default constructor */ }

ResponseCodes::ResponseCodes( const ResponseCodes& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

ResponseCodes::~ResponseCodes( void ) {	/* destructor */ } 

/* OPERATOR OVERLOADS */

ResponseCodes&	ResponseCodes::operator=( const ResponseCodes& rhs ) {

	if (this != &rhs ) {
		return (*this);
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */

std::string	ResponseCodes::getCodeStatusLine( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::intialize_codes();
	if (ResponseCodes::codes_[code].empty())
		return ("HTTP/1.1 500 Internal Server Error\r\n");
	std::string element = "HTTP/1.1 " + std::to_string(code) + " " + ResponseCodes::codes_[code] + "\r\n";
	return (element);
}

std::string	ResponseCodes::getCodeElementBody( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::intialize_codes();
	std::string message = ResponseCodes::codes_[code];
	if (message.empty())
		message = "Unknown Error";
	std::string element = "\r\n<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>" + std::to_string(code) + " ";
	element += message + "</title>\n</head>\n<body>\n<h1>" + std::to_string(code) + " " + message + "</h1>\n</body>\n</html>\r\n";
	return (element);
}

std::string	ResponseCodes::getCombinedStatusLineAndBody( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::intialize_codes();
	std::string element = ResponseCodes::getCodeStatusLine(code) + ResponseCodes::getCodeElementBody(code);
	return (element);
}

/* CLASS PRIVATE METHODS */

void	ResponseCodes::intialize_codes( void ) {

	ResponseCodes::codes_[501] = "Not Implemented";
	ResponseCodes::codes_[404] = "Not Found";
}