
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
		ResponseCodes::initialize_codes();
	if (ResponseCodes::codes_[code].empty())
		return ("HTTP/1.1 500 Internal Server Error\r\n");
	std::string element = "HTTP/1.1 " + std::to_string(code) + " " + ResponseCodes::codes_[code] + "\r\n";
	return (element);
}

std::string	ResponseCodes::getCodeElementBody( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::initialize_codes();
	std::string message = ResponseCodes::codes_[code];
	if (message.empty())
		message = "Unknown Error";
	std::string element = "\r\n<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>" + std::to_string(code) + " ";
	element += message + "</title>\n</head>\n<body>\n<h1>" + std::to_string(code) + " " + message + "</h1>\n</body>\n</html>\r\n";
	return (element);
}

std::string	ResponseCodes::getCombinedStatusLineAndBody( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::initialize_codes();
	std::string element = ResponseCodes::getCodeStatusLine(code) + ResponseCodes::getCodeElementBody(code);
	return (element);
}

/* CLASS PRIVATE METHODS */

void	ResponseCodes::initialize_codes( void ) {

	// Information responses
	ResponseCodes::codes_[100] = "Continue";
	ResponseCodes::codes_[101] = "Switching Protocols";
	ResponseCodes::codes_[102] = "Processing";
	ResponseCodes::codes_[103] = "Early Hints";

	// Successful responses
	ResponseCodes::codes_[200] = "OK";
	ResponseCodes::codes_[201] = "Created";
	ResponseCodes::codes_[202] = "Accepted";
	ResponseCodes::codes_[203] = "Non-Authoritative Information";
	ResponseCodes::codes_[204] = "No Content";
	ResponseCodes::codes_[205] = "Reset Content";
	ResponseCodes::codes_[206] = "Partial Content";
	ResponseCodes::codes_[207] = "Multi-Status";
	ResponseCodes::codes_[208] = "Already Reported";
	ResponseCodes::codes_[226] = "IM Used";

	// Redirection messages
	ResponseCodes::codes_[300] = "Multiple Choices";
	ResponseCodes::codes_[301] = "Moved Permanently";
	ResponseCodes::codes_[302] = "Found";
	ResponseCodes::codes_[303] = "See Other";
	ResponseCodes::codes_[304] = "Not Modified";
	ResponseCodes::codes_[305] = "Use Proxy";
	ResponseCodes::codes_[306] = "unused";
	ResponseCodes::codes_[307] = "Temporary Redirect";
	ResponseCodes::codes_[308] = "Permanent Redirect";

	// Client error responses
	ResponseCodes::codes_[400] = "Bad Request";
	ResponseCodes::codes_[401] = "Unauthorized";
	ResponseCodes::codes_[402] = "Payment Required";
	ResponseCodes::codes_[403] = "Forbidden";
	ResponseCodes::codes_[404] = "Not Found";
	ResponseCodes::codes_[405] = "Method Not Allowed";
	ResponseCodes::codes_[406] = "Not Acceptable";
	ResponseCodes::codes_[407] = "Proxy Authentication Required";
	ResponseCodes::codes_[408] = "Request Timeout";
	ResponseCodes::codes_[409] = "Conflict";
	ResponseCodes::codes_[410] = "Gone";
	ResponseCodes::codes_[411] = "Length Required";
	ResponseCodes::codes_[412] = "Precondition Failed";
	ResponseCodes::codes_[413] = "Payload Too Large ";
	ResponseCodes::codes_[414] = "URI Too Long";
	ResponseCodes::codes_[415] = "Unsupported Media Type";
	ResponseCodes::codes_[416] = "Range Not Satisfiable";
	ResponseCodes::codes_[417] = "Expectation Failed";
	ResponseCodes::codes_[418] = "I'm a teapot";
	ResponseCodes::codes_[421] = "Misdirected Request";
	ResponseCodes::codes_[422] = "Unprocessable Content";
	ResponseCodes::codes_[423] = "Locked";
	ResponseCodes::codes_[424] = "Failed Dependency";
	ResponseCodes::codes_[425] = "Too Early";
	ResponseCodes::codes_[426] = "Upgrade Required";
	ResponseCodes::codes_[428] = "Precondition Required";
	ResponseCodes::codes_[429] = "Too Many Requests";
	ResponseCodes::codes_[431] = "Request Header Fields Too Large";
	ResponseCodes::codes_[451] = "Unavaiable For Legal Reasons";

	// Server error responses
	ResponseCodes::codes_[500] = "Internal Server Error";
	ResponseCodes::codes_[501] = "Not Implemented";
	ResponseCodes::codes_[502] = "Bad Gateway";
	ResponseCodes::codes_[503] = "Service Unavailable";
	ResponseCodes::codes_[504] = "Gateway Timeout";
	ResponseCodes::codes_[505] = "HTTP Version Not Supported";
	ResponseCodes::codes_[506] = "Variant Also Negotiates";
	ResponseCodes::codes_[507] = "Insufficient Storage";
	ResponseCodes::codes_[508] = "Loop Detected";
	ResponseCodes::codes_[510] = "Not Extended";
	ResponseCodes::codes_[511] = "Network Authentification Required";

}