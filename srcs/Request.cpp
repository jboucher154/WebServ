
#include "Request.hpp"

/* CONSTRUCTORS */

Request::Request( void ) 
: size_(0), chunked_(false), keep_alive_(false), headers_complete(false), complete_(false) {

	/* default constructor */
}

Request::Request( const Request& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

Request::~Request( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

Request&	Request::operator=( const Request& rhs ) {

	if (this != &rhs ) {
		this->size_ = rhs.size_;
		this->chunked_ = rhs.chunked_;
		this->keep_alive_ = rhs.keep_alive_;
		this->request_line_ = rhs.request_line_;
		this->headers_ = rhs.headers_;
		this->body_ = rhs.body_;
		this->complete_ = rhs.complete_;
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */

/* EXAMPLES */
/* Request */
// Get /mysite/index.html HTTP/1.1\r\n
// Host: 10.101.101.10\r\n
// Accept: */*\r\n
// \r\n

/* Response */
// HTTP/1.1 200 OK\r\n
// Content-Length: 55\r\n
// Content-Type: text/html\r\n
// Last-Modified: Wed, 12 Aug 1998 15:03:50 GMT\r\n
// Accept-Ranges: bytes\r\n
// ETag: “04f97692cbd1:377”\r\n
// Date: Thu, 19 Jun 2008 19:29:07 GMT\r\n
// \r\n
// <55-character response>

void	Request::add( std::string to_add ) { //add to any part of the Request, will direct to correct private parsing function
	
	std::stringstream	ss(to_add);
	std::string			line;

	while (getline(ss, line, '\n')) { // do I need my own getline??
		line += "\n";
		if (this->request_line_.empty()) {
			this->parseRequestLine_(line);
		}
		else if (this->headers_complete == false) {
			if (line.compare(CRLF) == 0) {
				this->headers_complete = true;
				continue ;
			}
			else {
				this->parseHeader_(line);
			}
		}
		else if (this->headers_complete && line.compare(CRLF) == 0) {
			break ;
		}
		else {
			this->parseBody_(line);
		}
	}
	// std::cout << "Parsing complete!!!!" << std::endl;//
	this->complete_ = true;
}

void	Request::clear( void ) { //clear all data in the request

	this->size_ = 0;
	this->chunked_ = false;
	this->keep_alive_ = false;
	this->headers_complete = false;
	this->request_line_.clear();
	this->headers_.clear();
	this->body_.clear();
	this->complete_ = false;
}

/* CLASS PRIVATE METHODS */

void	Request::parseRequestLine_( std::string& to_parse ) {

	// std::cout << "REQUEST LINE: " << to_parse << std::endl;
	this->request_line_["test"] = to_parse;
}

//save specific values for easy access

void	Request::parseHeader_( std::string& to_parse ) {

	(void)to_parse;
	// std::cout << "HEADER: " << to_parse << std::endl;
}

void	Request::parseBody_( std::string& to_parse ) {

	(void)to_parse;
	// std::cout << "BODY: " << to_parse << std::endl;
}


bool	Request::getComplete( void ) const {

	return (this->complete_);
}