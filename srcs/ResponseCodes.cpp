
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
	ResponseCodes::codes_[E_CONTINUE] = "Continue";
	ResponseCodes::codes_[E_SWITCHING_PROTOCOLS] = "Switching Protocols";
	ResponseCodes::codes_[E_PROCESSING] = "Processing";
	ResponseCodes::codes_[E_EARLY_HINTS] = "Early Hints";

	// Successful responses
	ResponseCodes::codes_[E_OK] = "OK";
	ResponseCodes::codes_[E_CREATED] = "Created";
	ResponseCodes::codes_[E_ACCEPTED] = "Accepted";
	ResponseCodes::codes_[E_NON_AUTHORITATIVE_INFORMATION] = "Non-Authoritative Information";
	ResponseCodes::codes_[E_NO_CONTENT] = "No Content";
	ResponseCodes::codes_[E_RESET_CONTENT] = "Reset Content";
	ResponseCodes::codes_[E_PARTIAL_CONTENT] = "Partial Content";
	ResponseCodes::codes_[E_MULTI_STATUS] = "Multi-Status";
	ResponseCodes::codes_[E_ALREADY_REPORTED] = "Already Reported";
	ResponseCodes::codes_[E_IM_USED] = "IM Used";

	// Redirection messages
	ResponseCodes::codes_[E_MULTIPLE_CHOICES] = "Multiple Choices";
	ResponseCodes::codes_[E_MOVED_PERMANENTLY] = "Moved Permanently";
	ResponseCodes::codes_[E_FOUND] = "Found";
	ResponseCodes::codes_[E_SEE_OTHER] = "See Other";
	ResponseCodes::codes_[E_NOT_MODIFIED] = "Not Modified";
	ResponseCodes::codes_[E_USE_PROXY] = "Use Proxy";
	ResponseCodes::codes_[E_UNUSED] = "unused";
	ResponseCodes::codes_[E_TEMPORARY_REDIRECT] = "Temporary Redirect";
	ResponseCodes::codes_[E_PERMANENT_REDIRECT] = "Permanent Redirect";

	// Client error responses
	ResponseCodes::codes_[E_BAD_REQUEST] = "Bad Request";
	ResponseCodes::codes_[E_UNAUTHORIZED] = "Unauthorized";
	ResponseCodes::codes_[E_PAYMENT_REQUIRED] = "Payment Required";
	ResponseCodes::codes_[E_FORBIDDEN] = "Forbidden";
	ResponseCodes::codes_[E_NOT_FOUND] = "Not Found";
	ResponseCodes::codes_[E_METHOD_NOT_ALLOWED] = "Method Not Allowed";
	ResponseCodes::codes_[E_NOT_ACCEPTABLE] = "Not Acceptable";
	ResponseCodes::codes_[E_PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required";
	ResponseCodes::codes_[E_REQUEST_TIMEOUT] = "Request Timeout";
	ResponseCodes::codes_[E_CONFLICT] = "Conflict";
	ResponseCodes::codes_[E_GONE] = "Gone";
	ResponseCodes::codes_[E_LENGTH_REQUIRED] = "Length Required";
	ResponseCodes::codes_[E_PRECONDITION_FAILED] = "Precondition Failed";
	ResponseCodes::codes_[E_PAYLOAD_TOO_LARGE] = "Payload Too Large ";
	ResponseCodes::codes_[E_URI_TOO_LONG] = "URI Too Long";
	ResponseCodes::codes_[E_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
	ResponseCodes::codes_[E_RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
	ResponseCodes::codes_[E_EXPECTATION_FAILED] = "Expectation Failed";
	ResponseCodes::codes_[E_IM_A_TEAPOT] = "I'm a teapot";
	ResponseCodes::codes_[E_MISDIRECTED_REQUEST] = "Misdirected Request";
	ResponseCodes::codes_[E_UNPROCESSABLE_CONTENT] = "Unprocessable Content";
	ResponseCodes::codes_[E_LOCKED] = "Locked";
	ResponseCodes::codes_[E_FAILED_DEPENDENCY] = "Failed Dependency";
	ResponseCodes::codes_[E_TOO_EARLY] = "Too Early";
	ResponseCodes::codes_[E_UPGRADE_REQUIRED] = "Upgrade Required";
	ResponseCodes::codes_[E_PRECONDITION_REQUIRED] = "Precondition Required";
	ResponseCodes::codes_[E_TOO_MANY_REQUESTS] = "Too Many Requests";
	ResponseCodes::codes_[E_REQUEST_HEADER_FEILDS_TOO_LARGE] = "Request Header Fields Too Large";
	ResponseCodes::codes_[E_UNAVAILABLE_FOR_LEGAL_REAONSONS] = "Unavaiable For Legal Reasons";

	// Server error responses
	ResponseCodes::codes_[E_INTERNAL_SERVER_ERROR] = "Internal Server Error";
	ResponseCodes::codes_[E_NOT_IMPLEMENTED] = "Not Implemented";
	ResponseCodes::codes_[E_BAD_GATEWAY] = "Bad Gateway";
	ResponseCodes::codes_[E_SERVICE_UNAVAILABLLE] = "Service Unavailable";
	ResponseCodes::codes_[E_GATEWAY_TIMEOUT] = "Gateway Timeout";
	ResponseCodes::codes_[E_HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	ResponseCodes::codes_[E_VARIANT_ALSO_NEGOTIATES] = "Variant Also Negotiates";
	ResponseCodes::codes_[E_INSUFFICIENT_STORAGE] = "Insufficient Storage";
	ResponseCodes::codes_[E_LOOP_DETECTED] = "Loop Detected";
	ResponseCodes::codes_[E_NOT_EXTENDED] = "Not Extended";
	ResponseCodes::codes_[E_NETWORK_AUTHENTIFICATION_REQUIRED] = "Network Authentification Required";
}