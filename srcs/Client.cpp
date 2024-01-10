#include "Client.hpp"

// remove later?
#include <cstring>

/* CONSTRUCTORS */

Client::Client( void ) : response_(NULL), cgi_handler_(NULL) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	// this->latest_time_ = time(0);
	time(&this->latest_time_);

	// this->resetRequest();
	// this->resetResponse();

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
	// this->latest_time_ = time(0);
	time(&this->latest_time_);

	// this->resetRequest();
	// this->resetResponse();

	this->server_ = server;
	this->server_fd_ = server_fd;
	
	while (!this->cgi_handler_) {
		this->cgi_handler_ = new CgiHandler;
		if (!this->cgi_handler_)
			Logger::log(E_ERROR, COLOR_RED, "Client constructor new failure (CgiHandler), attempting again");
	}
}

/* copy constructor */
Client::Client( const Client& to_copy ) : response_(NULL), cgi_handler_(NULL) {

	*this = to_copy;
} 

/* DESTRUCTOR */

Client::~Client( void ) {
	
	if (this->cgi_handler_){
		delete this->cgi_handler_;
		this->cgi_handler_ = NULL;
	}
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

/*! \brief start the handling of a request that involves cgi 
*
*	Added by ssalmi
*
*	if error set CGI flag to zero?
*	TALK WITH JENNY about what to do in error cases!
*	NOTE: remove unused cases
*/
bool	Client::startCgiResponse( void ) {

	if (this->response_.getStatusCode() >= 400)
		return false;
	int	cgi_result = this->cgi_handler_->initializeCgi(*this);

	if (cgi_result != EXIT_SUCCESS) {
		this->response_.setStatusCode(cgi_result);
		return false ;
	}
	else {
		return true ;
	}
}

void	Client::finishCgiResponse( void ) {

	this->setLatestTimeForClientAndServer();
	int	result = this->cgi_handler_->cgiFinish(this->response_);
	std::string method = this->request_.getRequestLineValue("method");

	if (result != EXIT_SUCCESS)
		this->response_.setStatusCode(result);
}

/******************************** end of CGI methods ********************************/

// setters
void	Client::setLatestTimeForClientAndServer( void ) {
	
	time(&this->latest_time_);
	this->getServer()->setLatestServerTime();
}

void	Client::setServerAndFd( Server* new_server, int new_server_fd ) {

	if (new_server) {
		this->server_ = new_server;
		this->server_fd_ = new_server_fd;
		this->response_.setServer(this->server_);
	}
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

/*! \brief Get the response string
*
*	Added by ssalmi
*
*	if error set CGI flag to zero?
*	TALK WITH JENNY about what to do in error cases!
*
*	CHECK LATER; at the current moment the ouput of the cgi output is stored into vector<char>,
*	but this causes because getResponseString returns a string reference.
*	I'm not sure what I should do, as fixing this issue goes on Jenny's turf that I'm unfamiliar with.
*	
*	Azzar's solution was to use new with the cgi_output, which solves the immediate issue,
*	but the bigger original issue still has to be fixed,
*	which is that this function returns a string, but won't this cause issues with non-string data?
*	What I wonder is that should we return std::vector<char> reference so that the data will not be modified
*	in conversion to a string?
*
*	As a temporary band-aid solution I've created is to simply create a member variable for the CgiHandler where
*	the vector<char> is stored into, but we'll most likely want to change this later.
*/
const std::string&	Client::getResponseString( void ) {

	//any CGI stuff
	if (this->request_.getCgiFlag()) {
		return this->response_.buildAndGetResponsePhase2(this->getCgiHandler()->getCgiOutputAsString());
	}
	else {
		return this->response_.buildAndGetResponsePhase2();
	}
	// if (this->request_.getComplete())
	// 	return this->response_.buildAndGetResponsePhase2();
	// else
	// 	return ("");
}

void	Client::addToRequest( char* message, size_t bytes_read ) {
	
	this->request_.add(message, bytes_read);
}

void	Client::resetResponse( void ) {
	
	this->response_.clear();
}

void	Client::resetRequest( void ) {

	this->request_.clear();
}


/* CLASS PRIVATE METHODS */
