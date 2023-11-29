#include "Client.hpp"

// remove later?
#include <cstring>

/* CONSTRUCTORS */

Client::Client( void ) : response_(NULL), cgi_handler_(NULL) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	this->latest_time_ = time(0);

	this->resetRequest();
	this->resetResponse();

	this->server_ = NULL;
	this->server_fd_ = -1;

	while (!this->cgi_handler_) {
		this->cgi_handler_ = new CgiHandler;
		if (!this->cgi_handler_)
			Logger::log(E_ERROR, COLOR_RED, "Client constructor new failure (CgiHandler)");
	}
}

Client::Client( int server_fd, Server* server ) : response_(server), cgi_handler_(NULL) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	this->latest_time_ = time(0);

	this->resetRequest();
	this->resetResponse();

	this->server_ = server;
	this->server_fd_ = server_fd;

	std::cout << "stage 1" << std::endl;
	
	while (!this->cgi_handler_) {
		std::cout << "stage 2" << std::endl;
		this->cgi_handler_ = new CgiHandler;
		if (!this->cgi_handler_)
			Logger::log(E_ERROR, COLOR_RED, "Client constructor new failure (CgiHandler), attempting again");
	}
	std::cout << "stage 3" << std::endl;
}

Client::Client( const Client& to_copy ) : response_(NULL), cgi_handler_(NULL) {

	*this = to_copy;
	/* copy constructor */
} 

/* DESTRUCTOR */

Client::~Client( void ) {
	if (this->cgi_handler_)
		delete this->cgi_handler_;
	this->cgi_handler_ = NULL;
	/* destructor */
} 

/* OPERATOR OVERLOADS */

Client&	Client::operator=( const Client& rhs ) {

	if (this != &rhs) {
		this->fd_ = rhs.fd_;
		this->address_ = rhs.address_;
		this->latest_time_ = rhs.latest_time_;
		this->server_ = rhs.server_;
		this->server_fd_ = rhs.server_fd_;
		this->request_ = rhs.request_;
		this->response_ = rhs.response_;
		if (this->cgi_handler_) {
			delete this->cgi_handler_;
			this->cgi_handler_ = NULL;
		}
		this->cgi_handler_ = new CgiHandler;
		if (rhs.cgi_handler_) {
			*(this->cgi_handler_) = *(rhs.cgi_handler_);
		}
	}
	return *this;
}

/* CLASS PUBLIC METHODS */

/******************************** CGI methods (ssalmi) ********************************/

/*! \brief start the handling of a request that involves cgi (POLL-version) 
*
*	Added by ssalmi
*
*	if error set CGI flag to zero?
*	TALK WITH JENNY about what to do in error cases!
*/
bool	Client::SELECT_startCgiResponse( void ) {

	int	cgi_result = this->cgi_handler_->SELECT_initializeCgi(*this);

	switch (cgi_result)
	{	
		case E_CGI_SERVERERROR:
			this->response_.setStatusCode(500);
			break;

		case E_CGI_UNKNOWNMETHOD:
			// won't happen here, prob can be removed
			break;

		case E_CGI_NOTFOUND:
			// won't happen here, prob can be removed
			break;

		case E_CGI_NOPERMISSION:
			// won't happen here, prob can be removed
			break;

		case E_CGI_OK:
			return true;
			break;

		default:
			break;
	}
	return false;
}


void	Client::SELECT_finishCgiResponse( void ) {

	int result = this->cgi_handler_->SELECT_cgiFinish(this->response_);

	switch (result)
	{
	case E_CGI_SERVERERROR:
		// set up status code? and send to client?
		break;
	
	case E_CGI_OK:
		break;

	default:
		// set up error teapot code? seems to be just a Carlos thing...
		break;
	}
}


/*! \brief start the handling of a request that involves cgi 
*
*	Added by ssalmi
*
*	if error set CGI flag to zero?
*	TALK WITH JENNY about what to do in error cases!
*/
bool	Client::POLL_startCgiResponse( void ) {

	if (this->response_.getStatusCode() >= 400)
		return false;
	int	cgi_result = this->cgi_handler_->POLL_initializeCgi(*this);

	switch (cgi_result)
	{	
		case E_CGI_SERVERERROR:
			// set status as ?
			this->response_.setStatusCode(500);
			// error printing?
			break;

		case E_CGI_UNKNOWNMETHOD:
			// set status as ?
			// error printing?
			break;

		case E_CGI_NOTFOUND:
			// set status as ?
			// error printing?
			break;

		case E_CGI_NOPERMISSION:
			// set status as ?
			// error printing?
			break;

		case E_CGI_OK:
			return true;
			break;

		default:
			// set status as ? (teapot???)
			// error printing?
			break;
	}
	return false;
}

void	Client::POLL_finishCgiResponse( void ) {

	int	result = this->cgi_handler_->POLL_cgiFinish(this->response_);
	std::string method = this->request_.getRequestLineValue("method");

	switch (result) {
		case E_CGI_OK :
			if (method == "POST")
			this->response_.setStatusCode(201);
			else
				this->response_.setStatusCode(200);
			break ;

		case E_CGI_SERVERERROR :
			this->response_.setStatusCode(500);
			break ;
		
		default :
			break;
	}
}

/******************************** end of CGI methods ********************************/

// setters
void	Client::setLatestTime( void ) {
	
	this->latest_time_ = time(0);
}

// getters
int	Client::getFd( void ) const {

	return this->fd_;
}

time_t	Client::getLatestTime( void ) const {

	return this->latest_time_;
}

Server*	Client::getServer( void ) const {

	return this->server_;
}

int	Client::getServerFd( void ) const {

	return this->server_fd_;
}

std::string	Client::getClientHost() const {

	char		client_host[INET_ADDRSTRLEN];
	
	inet_ntop(this->address_.sin_family, (struct sockaddr*)&this->address_, client_host, INET_ADDRSTRLEN);
	return client_host;
}

struct sockaddr_in&	Client::getAddress( void ) {

	return this->address_;
}

CgiHandler*	Client::getCgiHandler( void ) {

	return this->cgi_handler_;
}

Request&	Client::getRequest( void ) {

	return this->request_;
}

Response&	Client::getResponse( void ) {

	return this->response_;
}

const std::string&	Client::getResponseString( void ) {

	//any CGI stuff
	if (this->request_.getCgiFlag()) {
		return this->cgi_handler_->getCgiOutput();
	}
	else {
		return this->response_.get();
	}
	// if (this->request_.getComplete())
	// 	return this->response_.get();
	// else
	// 	return ("");
}

void	Client::addToRequest( std::string message ) {
	
	this->request_.add(message);
}

void	Client::resetResponse( void ) {
	
	this->response_.clear();
}

void	Client::resetRequest( void ) {

	this->request_.clear();
}


/* CLASS PRIVATE METHODS */
