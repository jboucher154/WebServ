#include "Client.hpp"

/* CONSTRUCTORS */

/*! \brief default constructor for client class
*
*	Initializes response and cgi_handler to NULL in initialization list.
*	Other attributes zeroed and set to NULL in body. cgi_handler is 
*	allocated here.
*
*/
Client::Client( void ) : 
	response_(NULL), 
	cgi_handler_(NULL) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	time(&this->latest_time_);
	this->server_ = NULL;
	this->server_fd_ = -1;
	while (!this->cgi_handler_) {// should drop client connection if this fails
		this->cgi_handler_ = new CgiHandler;
		if (!this->cgi_handler_)
			Logger::log(E_ERROR, COLOR_RED, "Client constructor new failure (CgiHandler)");
	}
}

/*! \brief constructor with parameters for Client object
*
*	Initializes response with passed server and sets cgi_handler to NULL in initialization list.
*	Server and server fd are assigned and other values are zeroed.
*	cgi_handler is allocated here.
*
*	@param server_fd fd of server client connected to
*	@param server pointer to server that client connected to
*/
Client::Client( int server_fd, Server* server ) : response_(server), cgi_handler_(NULL) {

	this->fd_ = -1;
	memset(&this->address_, 0, sizeof(this->address_));
	time(&this->latest_time_);
	this->server_ = server;
	this->server_fd_ = server_fd;
	while (!this->cgi_handler_) {//
		this->cgi_handler_ = new CgiHandler;
		if (!this->cgi_handler_)
			Logger::log(E_ERROR, COLOR_RED, "Client constructor new failure (CgiHandler), attempting again");
	}
}

/*! \brief copy constructor for Client object, initializes response and cgi_handler to NULL
*
*	Calls assignment operator to copy values from client object to copy.	
*
*	@param to_copy Client object to copy to current instance
*/
Client::Client( const Client& to_copy ) : response_(NULL), cgi_handler_(NULL) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief default destructor for the client class
*
*	Deletes the allocation for the cgi_handler.
*/
Client::~Client( void ) {
	
	if (this->cgi_handler_){
		delete this->cgi_handler_;
		this->cgi_handler_ = NULL;
	}
} 

/* OPERATOR OVERLOADS */

/*! \brief	copy assignment operator for Client object
*
*	Copies all attirbutes from rhs Client object. If rhs cgi_handler is null, the
*	current instance's cgi_handler will be cleared.
*
*	@param rhs client object to copy values from
*	@return @b Client& current instance of client object
*/
Client&	Client::operator=( const Client& rhs ) {

	if (this != &rhs) {
		this->fd_ = rhs.fd_;
		this->address_ = rhs.address_;
		this->latest_time_ = rhs.latest_time_;
		this->server_ = rhs.server_;
		this->server_fd_ = rhs.server_fd_;
		this->request_ = rhs.request_;
		this->response_ = rhs.response_;
		// if (this->cgi_handler_) {
		// 	delete this->cgi_handler_;
		// 	this->cgi_handler_ = NULL;
		// }
		// this->cgi_handler_ = new CgiHandler;
		if (rhs.cgi_handler_) {
			*(this->cgi_handler_) = *(rhs.cgi_handler_);
		}
		else { //added
			this->cgi_handler_->ClearCgiHandler();
		}
	}
	return *this;
}

/* CLASS PUBLIC METHODS */

/******************************** CGI methods (ssalmi) ********************************/

/*! \brief start the handling of a request that involves cgi 
*
*	@author ssalmi
*
*	Checks if there has been an error detected already by the response. If not, will 
*	call the first of the functions for the cgi process, `intitializeCgi`.
*	If this function returns anything other than success, the response status code is set to
*	the result of it's call.
*	
*	@return @b bool false if error encountered, otherwise true
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

/*! \brief calls final steps of cgi process and sets error code as needed
*
*	Updates the activity time for client and server then calls the `cgiFinish` method
*	to run final portions of cgi process. If this method call results in error, the
*	response status code is set to the relevant HTTP status code.
*/
void	Client::finishCgiResponse( void ) {

	this->setLatestTimeForClientAndServer();
	int	result = this->cgi_handler_->cgiFinish(this->response_);
	if (result != EXIT_SUCCESS)
		this->response_.setStatusCode(result);
}

/******************************** SETTERS ********************************/

/*! \brief sets the latest activity time for client and server to `now`
*
*	Updates the t_time structs in client and server to the current time.
*	Used to track the respective activity timeouts.
*/
void	Client::setLatestTimeForClientAndServer( void ) {
	
	time(&this->latest_time_);
	this->getServer()->setLatestServerTime();
}

/*! \brief setter for assignmentof new server to Client Object
*
*	If pointer to new server isn't NULL, the new server and its fd are set
*	 to the current instances attributes.
*
*	@param new_server pointer to server object to change to
*	@param new_server_fd socket fd of new server object
*/
void	Client::setServerAndFd( Server* new_server, int new_server_fd ) {

	if (new_server) {
		this->server_ = new_server;
		this->server_fd_ = new_server_fd;
		this->response_.setServer(this->server_);
	}
}

/********************************* GETTERS **********************************/

/*! \brief getter for the client fd
*
*	@return @b int client's fd
*/
int	Client::getFd( void ) const {

	return this->fd_;
}

/*! \brief getter for the clients latest activity time
*
*	@return @b time_t the last time client activity occured
*/
time_t	Client::getLatestTime( void ) const {

	return this->latest_time_;
}

/*! \brief getter for client's server 
*
*	@return @b Server* pointer to client's server object
*/
Server*	Client::getServer( void ) const {

	return this->server_;
}

/*! \brief getter for the client's server's socket fd
*
*	@return @b int socket fd of server object of client
*/
int	Client::getServerFd( void ) const {

	return this->server_fd_;
}

/*! \brief getter for string represenation of client host
*
*	Populates the character array with the client ip information using
*	inet_ntop to translate the address.
*
*	@return @b std::string of the client ip
*/
std::string	Client::getClientHost() const {

	char		client_host[INET_ADDRSTRLEN];
	
	inet_ntop(this->address_.sin_family, (struct sockaddr*)&this->address_, client_host, INET_ADDRSTRLEN);
	return client_host;
}

/*! \brief getter for client address
*
*	@return @b struct @b sockaddr_in& reference to the client address struct
*/
struct sockaddr_in&	Client::getAddress( void ) {

	return this->address_;
}

/*! \brief getter for the CgiHandler object
*
*	@return @b CgiHandler* pointer to current client instances's cgi handler object
*/
CgiHandler*	Client::getCgiHandler( void ) {

	return this->cgi_handler_;
}

/*! \brief getter for the Request object
*
*	@return @b Request& reference to client's Request object
*/
Request&	Client::getRequest( void ) {

	return this->request_;
}

/*! \brief getter for the Response object
*
*	@return @b Response& reference to client's Response object
*/
Response&	Client::getResponse( void ) {

	return this->response_;
}

/********************************* OTHER PUBLIC METHODS **********************************/

/*! \brief getter for the response string
*
*	@author ssalmi
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

	if (this->request_.getCgiFlag()) {
		return this->response_.buildAndGetResponsePhase2(this->getCgiHandler()->getCgiOutputAsString());
	}
	else {
		return this->response_.buildAndGetResponsePhase2();
	}
}

/*! \brief 
*
*
*/
void	Client::addToRequest( char* message, size_t bytes_read ) {
	
	this->request_.add(message, bytes_read);
}

/*! \brief 
*
*
*/
void	Client::resetResponse( void ) {
	
	this->response_.clear();
}

/*! \brief 
*
*
*/
void	Client::resetRequest( void ) {

	this->request_.clear();
}
