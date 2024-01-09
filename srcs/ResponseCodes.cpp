
#include "ResponseCodes.hpp"

std::map<int, std::string> ResponseCodes::codes_;

/*! \brief ResponseCodes class default constructor
*
*	instanciates an instance of the ResponseCodes class.
*   This constructor is private.
*
*   @param no param.
*   @return no returns.
*
*/
ResponseCodes::ResponseCodes( void ) { 

}

/*! \brief ResponseCodes's class copy constructor
*       
*  This copy constructor calls on the operator overload to copy the instance. 
*  This copy constructor is private.
*  
*   @param to_copy, a refrance to an instance of the ResponseCodes class that is to be coppied.
*   @return no return.
*/
ResponseCodes::ResponseCodes( const ResponseCodes& to_copy ) {

	*this = to_copy;
} 

/*! \brief ResponseCodes's class destructor
*       
*  Destructs an instance of the class. 
*  This destructor is private.
*   @param no param.
*   @return no returns.
*/
ResponseCodes::~ResponseCodes( void ) {

} 

/*! \brief ResponseCodes's assignment operator overload
*       
*  Returns a pointer to the current instance. 
*  This assignment operator overload is private.
*
*   @param rhs, a refrance to an instance of the ResponseCodes class.
*   @return a refrance to an instance of the ResponseCodes class created(copied) out of rhs.
*/
ResponseCodes&	ResponseCodes::operator=( const ResponseCodes& rhs ) {

	return (*this);
}

/*! \brief creats an html element out of Response Code passed to it 
*
*	This static method generates an html element including the response code and the 
*   coresponding string, out of Response Code passed to it.
*   
*   @param code, which will be used to generate an html element out of.
*   @return element, an html text made out of the code passed to the method.
*
*/
std::string	ResponseCodes::getCodeStatusLine( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::initialize_codes();
	if (ResponseCodes::codes_[code].empty())
		return ("HTTP/1.1 500 Internal Server Error\r\n");
	std::string element = "HTTP/1.1 " + std::to_string(code) + " " + ResponseCodes::codes_[code] + "\r\n";
	
	return (element);
}

/*! \brief creats an html text out of Response Code passed to it 
*
*	This static method generates an html text out of Response Code passed to it.
*   
*   @param code, which will be used to generate an html text out of.
*   @return element, an html text made out of the code passed to the method.
*
*/
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

/*! \brief Returns a string made out of the elements returned from 
*    getCodeStatusLine + the element returned from getCodeElementBody of this class.
*
*
*   @param no param.
*   @return no returns.
*   @param code, which will be used to generate a string made out of 2 html elements.
*   @return element, combination of 2 other html elements made out of the code passed to the method.
*
*/
std::string	ResponseCodes::getCombinedStatusLineAndBody( int code ) {

	if (ResponseCodes::codes_.empty())
		ResponseCodes::initialize_codes();
	std::string element = ResponseCodes::getCodeStatusLine(code) + ResponseCodes::getCodeElementBody(code);
	
	return (element);
}

/*! \brief Initializes map of Response Codes 
*
*	Initializes map of Response Codes by assigning the corresponding string to Response Codes
*   This static method is private.
*   @param no param.
*   @return no returns.
*
*/
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