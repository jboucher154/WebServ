
#include "Request.hpp"

/* CONSTRUCTORS */

Request::Request( void ) 
: body_size_(0), body_len_received_(0), chunked_(false), keep_alive_(false), headers_complete(false), complete_(false), sever_error_(false) {

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
		this->body_size_ = rhs.body_size_;
		this->body_len_received_ = rhs.body_len_received_;
		this->chunked_ = rhs.chunked_;
		this->keep_alive_ = rhs.keep_alive_;
		this->request_line_ = rhs.request_line_;
		this->headers_ = rhs.headers_;
		this->body_ = rhs.body_;
		this->complete_ = rhs.complete_;
	}
	return (*this);
}

/************** CLASS PUBLIC METHODS **************/

/*! \brief add to any part of the Request, directs to correct private parsing function
*       
*
*  More details to be filled as project progresses.
*  
*/
void	Request::add( std::string to_add ) {
	
	std::stringstream	ss(to_add);
	std::string			line;

	try {
		while (getline(ss, line, '\n')) { // do I need my own getline??
			if (this->request_line_.empty()) {
				this->parseRequestLine_(line);
			}
			else if (this->headers_complete == false) {
				if (line.compare("\r") == 0) {
					this->headers_complete = true;
					continue ;
				}
				else {
					this->parseHeader_(line);
				}
			}
			else if (this->headers_complete && line.compare("\r") == 0) {
				break ;
			}
			else {
				this->parseBody_(line);
			}
		}
		this->setRequestAttributes();
		if (this->headers_complete && this->body_size_ == this->body_len_received_) {
			this->complete_ = true;
		}
	}
	catch (const std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, e.what());
		this->sever_error_ = true;
	}
	printRequest();//
}

/*! \brief clears all containers and resets all values to intial state
*       
*
*  More details to be filled as project progresses.
*  
*/
void	Request::clear( void ) {

	this->body_size_ = 0;
	this->body_len_received_ = 0;
	this->chunked_ = false;
	this->keep_alive_ = false;
	this->headers_complete = false;
	this->request_line_.clear();
	this->headers_.clear();
	this->body_.clear();
	this->complete_ = false;
	this->sever_error_ = false;
}

/*! \brief prints to standard output `REQUEST` followed by
*				each request element on a newline.
*
*  More details to be filled as project progresses.
*  
*/
void	Request::printRequest( void ) const {

	std::cout << "REQUEST: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->request_line_.begin(); it != this->request_line_.end(); it++) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "\nHeaders:\n";
	for (std::map<std::string, std::string>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); it++) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "\nBody:\n";
	for (std::vector<std::string>::const_iterator it = this->body_.begin(); it != this->body_.end(); it++) {
		std::cout << *it;;
	}
	if (!this->body_.empty())
		std::cout << std::endl;
}

/************** PUBLIC GETTERS **************/

size_t		Request::getBodySize( void ) const {
	
	return (this->body_size_);
}

size_t		Request::getBodyLengthReceived( void ) const {

	return (this->body_len_received_);
}

bool	Request::getChunked( void ) const {

	return (this->chunked_);
}

bool	Request::getKeepAlive( void ) const {

	return (this->keep_alive_);
}


bool	Request::getComplete( void ) const {

	return (this->complete_);
}

bool	Request::getServerError( void ) const {
	
	return (this->sever_error_);
}

std::string	Request::getRequestLineValue( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->request_line_.find(key);
	if (value ==  this->request_line_.end()) {
		return ("");
	}
	else {
		return (value->second);
	}
}

std::map<std::string, std::string>::const_iterator	Request::getHeaderBegin( void ) const {

	return (this->headers_.begin());
}


std::map<std::string, std::string>::const_iterator	Request::getHeaderEnd( void ) const {

	return (this->headers_.end());
}

std::string	Request::getHeaderValueByKey( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->headers_.find(key);
	if (value ==  this->headers_.end()) {
		return ("");
	}
	else {
		return (value->second);
	}
}

std::vector<std::string>::iterator	Request::getBodyBegin( void ) {

	return (this->body_.begin());
}

std::vector<std::string>::iterator	Request::getBodyEnd( void ) {
	
	return (this->body_.end());
}

/************** CLASS PRIVATE METHODS **************/

/************** PRIVATE SETTERS **************/


void	Request::setBodySize( void ) {

	std::string	content_length = this->headers_["Content-Length"];
	this->body_size_ = ft_stoi(content_length);
}

void	Request::setChunked( void ) {

	std::string	transfer_encoding = this->headers_["Transfer-Encoding"];
	if (transfer_encoding == "chunked") {
		this->chunked_ = true;
	}
	else {
		this->chunked_ = false;
	}
}

void	Request::setKeepAlive( void ) {

	std::string connection = this->headers_["Connection"];
	if (connection == "keep-alive") {
		this->keep_alive_ = true;
	}
	else {
		this->keep_alive_ = false;
	}
}

void	Request::setRequestAttributes( void ) {

	void	(Request::*setters[])(void) = { &Request::setKeepAlive, &Request::setChunked, &Request::setBodySize };
	for (int i = 0; i < 3; i++) { //get size of setters instead of 3
		(this->*setters[i])();
	}
}

void	Request::parseRequestLine_( std::string& to_parse ) {

	std::stringstream	ss(to_parse);
	std::string			part;

	if (ss.fail() || ss.bad()) {
		throw(std::runtime_error("Stringstream failed in parseRequestLine_"));
	}
	ss >> part;
	this->request_line_["method"] = part;
	ss >> part;
	this->request_line_["uri"] = part;
	ss >> part;
	this->request_line_["version"] = part;

}

/*! \brief parses key and value from request header.
*       
*
*  If a header already exists it will not overwrite the old value.
*  
*/
void	Request::parseHeader_( std::string& to_parse ) {

	std::stringstream	ss(to_parse);
	std::string			key;
	std::string			value;
	size_t				key_len;

	if (ss.fail() || ss.bad()) {
		throw(std::runtime_error("Stringstream failed in parseHeader_"));
	}
	getline(ss, key, ':');
	key_len = key.length();
	value = to_parse.substr(key_len + 2, to_parse.length() - key_len);
	value.pop_back();
	if (this->headers_[key].empty()) {
		this->headers_[key] = value;
	}
}

void	Request::parseBody_( std::string& to_parse ) {

	to_parse.pop_back();
	this->body_.push_back(to_parse);
	this->body_len_received_ += to_parse.length();
}
