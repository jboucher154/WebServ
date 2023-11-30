
#include "CgiHandler.hpp"

#include <signal.h>


CgiHandler::CgiHandler( void )
	:	cgi_output_(""),
		metavariables_(NULL),
		args_(NULL),
		path_(NULL),
		piping_successful_(false),
		forking_successful_(false),
		pid_(-1)
{
	this->pipe_in_[0] = -1;
	this->pipe_in_[1] = -1;
	this->pipe_out_[0] = -1;
	this->pipe_out_[1] = -1;
}

CgiHandler::CgiHandler( const CgiHandler& to_copy )
	:	cgi_output_(""),
		metavariables_(NULL),
		args_(NULL),
		path_(NULL),
		piping_successful_(false),
		forking_successful_(false),
		pid_(-1)
{

	*this = to_copy;
} 

CgiHandler::~CgiHandler( void ) {

	deleteAllocatedCStringArray(this->metavariables_);
	deleteAllocatedCStringArray(this->args_);
	if (this->path_ != NULL)
		delete [] this->path_;
} 

/* OPERATOR OVERLOADS */

CgiHandler&	CgiHandler::operator=( const CgiHandler& rhs ) {

	if (this != &rhs) {
		this->metavariables_map_ = rhs.metavariables_map_;
		this->cgi_map_ = rhs.cgi_map_;

		if (rhs.metavariables_ != NULL)
			this->metavariables_ = copyCStringArray(rhs.metavariables_);	// do somekind of check here, throw exception?
		else
			this->metavariables_ = NULL;

		if (rhs.args_ != NULL)
			this->args_ = copyCStringArray(rhs.args_);						// same?
		else
			this->args_ = NULL;
		
		this->path_ = ft_strdup(rhs.path_);
		this->piping_successful_ = rhs.piping_successful_;
		this->forking_successful_ = rhs.forking_successful_;
		for (int i = 0; i < 2; ++i) {
			this->pipe_in_[i] = rhs.pipe_in_[i];
			this->pipe_out_[i] = rhs.pipe_out_[i];
		}
		this->pid_ = rhs.pid_;
	}

	return *this;
}

/* CLASS PUBLIC METHODS */

/*! \brief Prepare CgiHandler for next CGI call.
*       
*	This function does not clear the cgi_output_!
*/
void	CgiHandler::ClearCgiHandler( void ) {
	
	this->metavariables_map_.clear();
	this->cgi_map_.clear();
	deleteAllocatedCStringArray(this->metavariables_);
	this->metavariables_ = NULL;
	deleteAllocatedCStringArray(this->args_);
	this->args_ = NULL;
	this->path_ = NULL;
	this->piping_successful_ = false;
	this->forking_successful_ = false;
	this->closeCgiPipes();
	for (int i = 0; i < 2; ++i) {
		this->pipe_in_[i] = -1;
		this->pipe_out_[i] = -1;
	}
	this->pid_ = -1;
}

/*! \brief Close all pipes used for CGI.
*       if the integer is a file descriptor and it is open, close the file descriptor.
*/
void	CgiHandler::closeCgiPipes( void ) {

	if (fcntl(this->pipe_in_[0], F_GETFD) != -1)
		if (fcntl(this->pipe_in_[0], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_in_[0]);
	if (fcntl(this->pipe_in_[1], F_GETFD) != -1)
		if (fcntl(this->pipe_in_[1], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_in_[1]);
	if (fcntl(this->pipe_out_[0], F_GETFD) != -1)
		if (fcntl(this->pipe_out_[0], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_out_[0]);
	if (fcntl(this->pipe_out_[1], F_GETFD) != -1)
		if (fcntl(this->pipe_out_[1], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_out_[1]);
}

void	CgiHandler::clearCgiHandlerPollfdsAndSets( void ) {

	this->pollfds_.clear();
	FD_ZERO(&this->read_fd_set_);
	FD_ZERO(&this->write_fd_set_);
}

/****************************** SELECT methods ******************************/

int	CgiHandler::SELECT_initializeCgi( Client& client ) {

	std::string uri = client.getRequest().getRequestLineValue("uri");

	this->path_ = const_cast<char*>(client.getResponse().getResourcePath().c_str());

	int 	result;

	/* Request has already checked that cgi-location is valid,
	that the server has the permission,
	that you can access the cgi script/program etc. */

	if ((result = this->fillMetavariablesMap_(client)) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}
	this->metavariables_ = this->convertMetavariablesMapToCStringArray_();
	if (this->metavariables_ == NULL) {
		this->ClearCgiHandler();
		return E_CGI_SERVERERROR;
	}
	if ((result = this->createCgiArguments_(uri)) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}
	if ((result = this->SELECT_setUpCgiPipes_()) != E_CGI_OK) {
		this->ClearCgiHandler();
	}
	return result;
}


int	CgiHandler::SELECT_cgiFinish( Response& response ) {

	int	result;

	if ((result = this->SELECT_executeCgi_(response.getFileDataBegin(), response.getFileDataEnd())) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}

	result = this->SELECT_storeCgiOutput_();
	this->ClearCgiHandler();
	return result;
}

/****************************** POLL methods ******************************/

int	CgiHandler::POLL_initializeCgi( Client& client ) {

	std::string uri = client.getRequest().getRequestLineValue("uri");

	this->path_ = const_cast<char*>(client.getResponse().getResourcePath().c_str());

	int 	result;

	/* Request has already checked that cgi-location is valid,
	that the server has the permission,
	that you can access the cgi script/program etc. */

	if ((result = this->fillMetavariablesMap_(client)) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}
	this->metavariables_ = this->convertMetavariablesMapToCStringArray_();
	if (this->metavariables_ == NULL) {
		this->ClearCgiHandler();
		return E_CGI_SERVERERROR;
	}
	if ((result = this->createCgiArguments_(uri)) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}
	if ((result = this->POLL_setUpCgiPipes_()) != E_CGI_OK) {
		this->ClearCgiHandler();
	}
	return result;
}

int	CgiHandler::POLL_cgiFinish( Response& response ) {

	int	result;

	if ((result = this->POLL_executeCgi_(response.getFileDataBegin(), response.getFileDataEnd())) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}

	result = this->POLL_storeCgiOutput_();
	this->ClearCgiHandler();
	return result;
}

/* CLASS PRIVATE METHODS */

void	CgiHandler::SELECT_cgiRemoveFdFromSets_( int fd ) {
	
	if (FD_ISSET(fd, &this->read_fd_set_))
		FD_CLR(fd, &this->read_fd_set_);
	if (FD_ISSET(fd, &this->write_fd_set_))
		FD_CLR(fd, &this->write_fd_set_);
}

void	CgiHandler::SELECT_cgiRemoveCgiPipeEndsFromSets_( int pipe_in, int pipe_out ) {

	if (FD_ISSET(pipe_in, &this->read_fd_set_))
		FD_CLR(pipe_in, &this->read_fd_set_);
	if (FD_ISSET(pipe_out, &this->write_fd_set_))
		FD_CLR(pipe_out, &this->write_fd_set_);
}

void	CgiHandler::SELECT_cgiAddFdToReadSet_( int fd ) {

	if (!FD_ISSET(fd, &this->read_fd_set_))
		FD_SET(fd, &this->read_fd_set_);
}

void	CgiHandler::SELECT_cgiAddFdToWriteSet_( int fd ) {

	if (!FD_ISSET(fd, &this->write_fd_set_))
		FD_SET(fd, &this->write_fd_set_);
}

void	CgiHandler::POLL_cgiRemoveFdFromPollfds_( int fd ) {

	for (std::vector<pollfd>::iterator it = this->pollfds_.begin(); it != this->pollfds_.end(); ++it) {
		if (it->fd == fd) {
			this->pollfds_.erase(it);
			break;
		}
	}
}

void	CgiHandler::POLL_cgiRemoveCgiPipeEndsFromPollfds_( int pipe_in, int pipe_out ) {
	
	this->POLL_cgiRemoveFdFromPollfds_(pipe_in);
	this->POLL_cgiRemoveFdFromPollfds_(pipe_out);
}

/*! \brief Add a file descriptor to the pollfds vector.
*		Mode should be either POLLIN or POLLOUT!
*/
void	CgiHandler::POLL_cgiAddFdtoPollfds_( int fd, int mode ) {

	pollfd new_pollfd = {fd, mode, 0};
	this->pollfds_.push_back(new_pollfd);
}

/*! \brief Fill metavariables_map_ with key/value-data. Later on the map will be converted into a
*			c-style string array which will be passed to the cgi script as the environment.
*			Returns a e_cgi_results enum value which tells if the function was successful or not.
*       
*	ASK JENNY. IS request.getHeaderValueByKey case sensitive? Is "user-agent" the same as "User-Agent"?
*/
int	CgiHandler::fillMetavariablesMap_( Client& client ) {

	Request& request = client.getRequest();

	this->metavariables_map_["AUTH_TYPE"] = "Basic";	// check this later ("Digest" and "Form" are other common values)

	this->metavariables_map_["DOCUMENT_ROOT"] = client.getServer()->getRoot();
	this->metavariables_map_["HTTP_USER_AGENT"] = request.getHeaderValueByKey("User-Agent");
	this->metavariables_map_["HTTP_COOKIE"] = request.getHeaderValueByKey("Cookie");
	this->metavariables_map_["HTTP_REFERER"] = request.getHeaderValueByKey("Referer");
	this->metavariables_map_["GATEWAY_INTERFACE"] = "CGI/1.1";

	this->metavariables_map_["SERVER_NAME"] = client.getServer()->getServerName();
	this->metavariables_map_["SERVER_PORT"] = client.getServer()->getListeningPortInt();
	this->metavariables_map_["SERVER_PROTOCOL"] = "http/1.1";
	this->metavariables_map_["SERVER_SOFTWARE"] = "JAS-webserver/0.75"; // come up with server name (JAS would be great!)

	this->metavariables_map_["REMOTE_HOST"] = client.getClientHost();
	this->metavariables_map_["REMOTE_PORT"] = ntohs(client.getAddress().sin_port);
	this->metavariables_map_["PATH_INFO"] = request.getRequestLineValue("uri");
	this->metavariables_map_["PATH_TRANSLATED"] = this->path_;
	this->metavariables_map_["QUERY_STRING"] = client.getResponse().getQueryString();

	return E_CGI_OK;
}

/*! \brief convert the metavariables_map_ to a NULL-terminating c-style string array.
*		if Succeeds, will return a pointer to the string array, else returns a NULL pointer.
*       
*	the function finds out the map size and will allocate an array of that size + 1.
*	After that it will iterate through the map and store the values in a std::string
*	as the following: it->first + "=" + it->second. After that the string will be passed to
*	ft_strdup which will return a string pointer. If the returned pointer has a NULL value,
*	we delete the string array by calling deleteAllocatedCStringArray and returning NULL.
*
*	After iterating through the map we set the final pointer to NULL and return the string array.
*/
char**	CgiHandler::convertMetavariablesMapToCStringArray_( void ) {

	char**	string_array = NULL;
	size_t	map_size = this->metavariables_map_.size();

	try {
		string_array = new char*[map_size + 1];
	} catch (std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "convertMetavariablesMapToCStringArray_: %s", e.what());
		return NULL;
	}

	int	i = 0;
	std::string	metavariables_string;

	for (std::map<std::string, std::string>::iterator it = this->metavariables_map_.begin(); it != this->metavariables_map_.end(); ++it) {
		metavariables_string = it->first + "=" + it->second;
		string_array[i] = ft_strdup(metavariables_string.c_str());
		if (string_array[i] == NULL) {
			deleteAllocatedCStringArray(string_array);
			return NULL;
		}
		i++;
	}
	string_array[i] = NULL;

	return string_array;
}

/*! \brief
*
*
*
*
*/
std::string	CgiHandler::getExtension( std::string uri ) {

	size_t		extension_start = uri.find_last_of('.');
	std::string	extension;

	if (extension_start == std::string::npos) {
		extension = "cgi";
	}
	else {
		extension = uri.substr(extension_start + 1);
	}
	return extension;
}

/*! \brief Create the arguments for running the CGI script.
*
*	First we find out if the script's extension is ".cgi" and set the size
*	of the args_ array to either 1 or 2 (+1). If the extension is ".cgi", we do not need
*	a program to run the script and can set the script as the first and only arg,
*	 but otherwise we need the path to the program to run the script
*	(for example if bash, args_[0] should be "bin/bash"), and set the path to the
*	actual script as the second argument. Last arg has to be NULL!
*/
int	CgiHandler::createCgiArguments_( std::string uri ) {
	
	int size;
	std::string	extension = this->getExtension(uri);

	try {
		if (extension == ".cgi") // get extension, if ".cgi" malloc an string array of 2, else 3
			size = 1;
		else
			size = 2;
		this->args_ = new char*[size + 1];
		this->args_[size] = NULL;
	} catch (std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "CreateCgiArguments allocation error: %s", e.what());
		return E_CGI_SERVERERROR;
	}
	try {
		if (size == 1)
			this->args_[0] = ft_strdup(this->path_);
		else {
			// args_[0] = extension executable, for example /bin/bash"
			this->args_[0] = ft_strdup("/bin/bash"); 	// THIS IS HARDCODED, CHANGE LATER, MOFO!
			this->args_[1] = ft_strdup(this->path_);
		}
	} catch(std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "strdup error: %s", e.what());
		deleteAllocatedCStringArray(this->args_);
		this->args_ = NULL;
		return E_CGI_SERVERERROR;
	}
	return E_CGI_OK;
}

void	CgiHandler::cgiTimer_( int& status ) {

	time_t	start = time(NULL);
	int		result;

	while (difftime(time(NULL), start) >= CGI_TIMEOUT) {
		result = waitpid(this->pid_, &status, WNOHANG);
		if (result > 0)
			return;
	}

	kill(this->pid_, SIGKILL);	// if it got here the CGI process has gone on for too long, close it remotely
	waitpid(this->pid_, &status, 0);
}

/****************************** getters ******************************/

const std::string&	CgiHandler::getCgiOutput( void ) const {
	
	return this->cgi_output_;
}

std::vector<pollfd>&	CgiHandler::getCgiPollfds( void ) {

	return this->pollfds_;
}

fd_set&					CgiHandler::getCgiReadSet( void ) {

	return this->read_fd_set_;
}

fd_set&					CgiHandler::getCgiwriteSet( void ) {

	return this->write_fd_set_;
}

/****************************** SELECT methods ******************************/

int	CgiHandler::SELECT_setUpCgiPipes_( void ) {

	if (pipe(pipe_in_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	if (pipe(pipe_out_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->closeCgiPipes();
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	if (fcntl(this->pipe_in_[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 || fcntl(this->pipe_out_[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes fcntl error: %s", strerror(errno));
		this->closeCgiPipes();
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	
	this->SELECT_cgiAddFdToReadSet_(this->pipe_in_[1]);
	this->SELECT_cgiAddFdToWriteSet_(this->pipe_out_[0]);

	this->piping_successful_ = true;
	return E_CGI_OK;
}

/*! \brief send body-info to pipe, fork a process and execute cgi script.
*
*	Talk with Jenny about how to get the bodyinfo into a string or an array of characters!
*/
int	CgiHandler::SELECT_executeCgi_( std::vector<std::string>::iterator it_start, std::vector<std::string>::iterator it_end ) {

	(void)it_start;
	(void)it_end;
	std::string	body_string = "test";	// get body into string

	if (body_string.empty())
		send(this->pipe_in_[1], "\0", 1, 0);
	else
		send(this->pipe_in_[1], body_string.c_str(), body_string.length(), 0);
	
	// server_manager.SELECT_removeFdFromSets(this->pipe_in_[1]);
	close(pipe_in_[1]);
	
	if ((this->pid_ = fork()) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fork failure: %s", strerror(errno));
		this->forking_successful_ = false;
		this->closeCgiPipes();
		return E_CGI_SERVERERROR;
	}
	if (this->pid_ == 0) { // child process
		this->forking_successful_ = true;
		dup2(this->pipe_out_[1], STDOUT_FILENO);
		dup2(this->pipe_in_[0], STDIN_FILENO);
		this->closeCgiPipes();

		execve(this->path_, this->args_, this->metavariables_);

		Logger::log(E_ERROR, COLOR_RED, "execve error: %s", strerror(errno));	// if we get here there was an error in execve!
		delete [] this->path_;
		deleteAllocatedCStringArray(this->args_);
		deleteAllocatedCStringArray(this->metavariables_);
		std::exit(EXIT_FAILURE);
	} else {	// parent process
		this->forking_successful_ = true;
		close(this->pipe_out_[1]);

		int status;
		this->cgiTimer_(status);
		if (WIFSIGNALED(status) > 0 || WEXITSTATUS(status) != 0)
			return E_CGI_SERVERERROR;
	}

	return E_CGI_OK;
}

/*! \brief Store cgi output into a string that will be sent to the client.
*
*	Most likely will need heavy refactoring. CREATE MACROS!
*/
int	CgiHandler::SELECT_storeCgiOutput_( void ) {
	
	int	ret = 1;
	int	bytesread = 0;
	char	buffer[10000]; // change buffer size later; make a macro for it!

	this->cgi_output_.clear();
	memset(buffer, 0, 10000);

	while (ret > 0) {
		ret = recv(this->pipe_out_[0], buffer, 1024, 0); // set up a read_max
		if (ret > 0) {
			this->cgi_output_.append(buffer);
			bytesread += ret;
		}
		if (bytesread >= 10000) {
			this->closeCgiPipes();
			return E_CGI_SERVERERROR;
		}
	}
	if (ret < 0) {
		Logger::log(E_ERROR, COLOR_RED, "storeCgiOutput: ", strerror(errno));
		this->closeCgiPipes();
		return E_CGI_SERVERERROR;
	}
	this->closeCgiPipes();

	return E_CGI_OK;
}

/****************************** POLL methods ******************************/

int	CgiHandler::POLL_setUpCgiPipes_( void ) {

	if (pipe(pipe_in_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	if (pipe(pipe_out_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->closeCgiPipes();
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	if (fcntl(this->pipe_in_[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 || fcntl(this->pipe_out_[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes fcntl error: %s", strerror(errno));
		this->closeCgiPipes();
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}

	this->POLL_cgiAddFdtoPollfds_(this->pipe_in_[1], POLLIN);
	this->POLL_cgiAddFdtoPollfds_(this->pipe_out_[0], POLLOUT);

	this->piping_successful_ = true;
	return E_CGI_OK;
}

/*! \brief send body-info to pipe, fork a process and execute cgi script.
*
*	Talk with Jenny about how to get the bodyinfo into a string or an array of characters!
*/
int		CgiHandler::POLL_executeCgi_( std::vector<std::string>::iterator it_start, std::vector<std::string>::iterator it_end ) {

	(void)it_start;
	(void)it_end;
	std::string	body_string = "";	// get body into string

	std::cout << "POLL_executeCgi_" << std::endl;

	
	// server_manager.POLL_removeFdFromPollfds(this->pipe_in_[1]);
	close(pipe_in_[1]);
	
	if ((this->pid_ = fork()) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fork failure: %s", strerror(errno));
		this->forking_successful_ = false;
		this->closeCgiPipes();
		return E_CGI_SERVERERROR;
	}
	if (this->pid_ == 0) { // child process
		this->forking_successful_ = true;
		dup2(this->pipe_out_[1], STDOUT_FILENO);
		dup2(this->pipe_in_[0], STDIN_FILENO);
		this->closeCgiPipes();

		if (body_string.empty())
			send(this->pipe_in_[1], "\0", 1, 0);
		else
			send(this->pipe_in_[1], body_string.c_str(), body_string.length(), 0);

		// execve(this->path_, this->args_, this->metavariables_);

		Logger::log(E_ERROR, COLOR_RED, "execve error: %s", strerror(errno));	// if we get here there was an error in execve!
		delete [] this->path_;
		deleteAllocatedCStringArray(this->args_);
		deleteAllocatedCStringArray(this->metavariables_);
		std::exit(EXIT_FAILURE);
	} else {	// parent process
		this->forking_successful_ = true;
		close(this->pipe_out_[1]);

		int status;
		this->cgiTimer_(status);
		if (WIFSIGNALED(status) > 0 || WEXITSTATUS(status) != 0)
			return E_CGI_SERVERERROR;
	}

	return E_CGI_OK;
}

/*! \brief Store cgi output into a string that will be sent to the client.
*
*	Most likely will need heavy refactoring. CREATE MACROS!
*/
int		CgiHandler::POLL_storeCgiOutput_( void ) {

	int	ret = 1;
	int	bytesread = 0;
	char	buffer[10000]; // change buffer size later; make a macro for it!

	this->cgi_output_.clear();
	memset(buffer, 0, 10000);

	while (ret > 0) {
		ret = recv(this->pipe_out_[0], buffer, 1024, 0); // set up a read_max
		if (ret > 0) {
			this->cgi_output_.append(buffer);
			bytesread += ret;
		}
		if (bytesread >= 10000) {
			this->closeCgiPipes();
			return E_CGI_SERVERERROR;
		}
	}
	if (ret < 0) {
		Logger::log(E_ERROR, COLOR_RED, "storeCgiOutput: ", strerror(errno));
		this->closeCgiPipes();
		return E_CGI_SERVERERROR;
	}
	this->closeCgiPipes();

	return E_CGI_OK;
}
