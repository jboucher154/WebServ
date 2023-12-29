
#include "CgiHandler.hpp"

#include <signal.h>

CgiHandler::CgiHandler( void )
	:	cgi_output_as_string_(""),
		metavariables_(NULL),
		args_(NULL),
		path_(NULL),
		piping_successful_(false),
		forking_successful_(false),
		pid_(-1)
{
	this->pipe_into_cgi_[0] = -1;
	this->pipe_into_cgi_[1] = -1;
	this->pipe_from_cgi_[0] = -1;
	this->pipe_from_cgi_[1] = -1;
}

CgiHandler::CgiHandler( const CgiHandler& to_copy )
	:	cgi_output_as_string_(""),
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

	this->closeCgiPipes();
	deleteAllocatedCStringArray(this->metavariables_);
	deleteAllocatedCStringArray(this->args_);
	if (this->path_ != NULL)
		delete [] this->path_;
} 

/* OPERATOR OVERLOADS */

CgiHandler&	CgiHandler::operator=( const CgiHandler& rhs ) {

	if (this != &rhs) {
		this->metavariables_map_ = rhs.metavariables_map_;
		// this->cgi_map_ = rhs.cgi_map_;

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
			this->pipe_into_cgi_[i] = rhs.pipe_into_cgi_[i];
			this->pipe_from_cgi_[i] = rhs.pipe_from_cgi_[i];
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
	// this->cgi_map_.clear();
	deleteAllocatedCStringArray(this->metavariables_);
	this->metavariables_ = NULL;
	deleteAllocatedCStringArray(this->args_);
	this->args_ = NULL;
	this->path_ = NULL;
	this->piping_successful_ = false;
	this->forking_successful_ = false;
	this->closeCgiPipes();
	for (int i = 0; i < 2; ++i) {
		this->pipe_into_cgi_[i] = -1;
		this->pipe_from_cgi_[i] = -1;
	}
	this->pid_ = -1;
}

/*! \brief Close all pipes used for CGI.
*       if the integer is a file descriptor and it is open, close the file descriptor.
*/
void	CgiHandler::closeCgiPipes( void ) {

	if (fcntl(this->pipe_into_cgi_[0], F_GETFD) != -1)
		if (fcntl(this->pipe_into_cgi_[0], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_into_cgi_[0]);
	if (fcntl(this->pipe_into_cgi_[1], F_GETFD) != -1)
		if (fcntl(this->pipe_into_cgi_[1], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_into_cgi_[1]);
	if (fcntl(this->pipe_from_cgi_[0], F_GETFD) != -1)
		if (fcntl(this->pipe_from_cgi_[0], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_from_cgi_[0]);
	if (fcntl(this->pipe_from_cgi_[1], F_GETFD) != -1)
		if (fcntl(this->pipe_from_cgi_[1], F_GETFL) != -1 || errno != EBADF)
			close (this->pipe_from_cgi_[1]);
}

int	CgiHandler::initializeCgi( Client& client ) {

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
	if ((result = this->createCgiArguments_(uri, client)) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}
	if ((result = this->setUpCgiPipes_()) != E_CGI_OK) {
		this->ClearCgiHandler();
	}
	return result;
}

int	CgiHandler::cgiFinish( Response& response ) {

	int	result;

	if ((result = this->executeCgi_(response.getFileDataBegin(), response.getFileDataEnd())) != E_CGI_OK) {
		this->ClearCgiHandler();
		return result;
	}

	result = this->storeCgiOutput_();
	this->ClearCgiHandler();
	return result;
}

/*! \brief This function handles clearing of the cgi outputs. 
*
*	BE AWARE: I've created cgi_output_as_string, but if we don't go with this solution just remove it and
*	implement these changes to ServerManager::sendResponseToClient and Client::getResponseString.
*/
void	CgiHandler::clearCgiOutputs( void ) {

	// this->cgi_output_.clear();
	this->cgi_output_as_string_ = "";
}

/* CLASS PRIVATE METHODS */


/*! \brief Fill metavariables_map_ with key/value-data. Later on the map will be converted into a
*			c-style string array which will be passed to the cgi script as the environment.
*			Returns a e_cgi_results enum value which tells if the function was successful or not.
*       
*/
int	CgiHandler::fillMetavariablesMap_( Client& client ) {

	Request&	request = client.getRequest();
	Server&		server = *(client.getServer());

	this->metavariables_map_["AUTH_TYPE"] = "Basic";	// check this later ("Digest" and "Form" are other common values)

	this->metavariables_map_["DOCUMENT_ROOT"] = server.getRoot();
	this->metavariables_map_["HTTP_USER_AGENT"] = request.getHeaderValueByKey("User-Agent");
	this->metavariables_map_["HTTP_COOKIE"] = request.getHeaderValueByKey("Cookie");
	this->metavariables_map_["HTTP_REFERER"] = request.getHeaderValueByKey("Referer");
	this->metavariables_map_["PATH_INFO"] = request.getRequestLineValue("uri");
	this->metavariables_map_["PATH_TRANSLATED"] = this->path_;
	this->metavariables_map_["QUERY_STRING"] = client.getResponse().getQueryString();
	this->metavariables_map_["GATEWAY_INTERFACE"] = "CGI/1.1";

	this->metavariables_map_["SERVER_NAME"] = server.getServerName();
	this->metavariables_map_["SERVER_PORT"] = server.getListeningPortInt();
	this->metavariables_map_["SERVER_PROTOCOL"] = "http/1.1";
	this->metavariables_map_["SERVER_SOFTWARE"] = "JAS-webserver/0.75"; // come up with server name (JAS would be great!)

	this->metavariables_map_["REMOTE_HOST"] = client.getClientHost();
	this->metavariables_map_["REMOTE_PORT"] = ntohs(client.getAddress().sin_port);

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

/****************************** getters ******************************/

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
		extension = ".cgi";
	}
	else {
		extension = uri.substr(extension_start);
	}
	return extension;
}

// const std::vector<char>&	CgiHandler::getCgiOutput( void ) const {
	
// 	return this->cgi_output_;
// }

const std::string&	CgiHandler::getCgiOutputAsString_( void ) const {

	return this->cgi_output_as_string_;
}

char**	CgiHandler::getArgs( void ) const {

	return this->args_;
}

const int*	CgiHandler::getPipeIn( void ) const {

	return this->pipe_into_cgi_;
}

const int*	CgiHandler::getPipeOut( void ) const {

	return this->pipe_from_cgi_;
}



/* CLASS PRIVATE METHODS */

/*! \brief Create the arguments for running the CGI script.
*
*	First we find out if the script's extension is ".cgi" and set the size
*	of the args_ array to either 1 or 2 (+1). If the extension is ".cgi", we do not need
*	a program to run the script and can set the script as the first and only arg,
*	 but otherwise we need the path to the program to run the script
*	(for example if bash, args_[0] should be "bin/bash"), and set the path to the
*	actual script as the second argument. Last arg has to be NULL!
*/
int	CgiHandler::createCgiArguments_( std::string uri, Client& client ) {
	
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
		if (size == 1){
			this->args_[0] = ft_strdup(this->path_);
		}
		else {
			this->args_[0] = ft_strdup(client.getServer()->getCgiExecutor(extension).c_str());	//extension executable, for example "/bin/bash" or "/usr/local/bin/python3"
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

	while (difftime(time(NULL), start) <= CGI_TIMEOUT) {
		result = waitpid(this->pid_, &status, WNOHANG);
		if (result > 0)
			return;
	}

	kill(this->pid_, SIGKILL);	// if it got here the CGI process has gone on for too long, close it remotely
	waitpid(this->pid_, &status, 0);
}

int	CgiHandler::setUpCgiPipes_( void ) {

	if (pipe(pipe_into_cgi_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	if (pipe(pipe_from_cgi_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->closeCgiPipes();
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}
	if (fcntl(this->pipe_into_cgi_[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 || fcntl(this->pipe_from_cgi_[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes fcntl error: %s", strerror(errno));
		this->closeCgiPipes();
		this->piping_successful_ = false;
		return E_CGI_SERVERERROR;
	}

	this->piping_successful_ = true;
	return E_CGI_OK;
}

/*! \brief send body-info to pipe, fork a process and execute cgi script.
*
*	Talk with Jenny about how to get the bodyinfo into a string or an array of characters!
*/
int		CgiHandler::executeCgi_( std::vector<std::string>::iterator it_start, std::vector<std::string>::iterator it_end ) {

	std::string	body_string = "";	// get body into std::string (or c-style char array; TALK WITH JENNY)
									//may need to track body size separately

	for (std::vector<std::string>::iterator it = it_start; it != it_end; ++it) {
		body_string += *it;
	}

	if ((this->pid_ = fork()) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fork failure: %s", strerror(errno));
		this->forking_successful_ = false;
		this->closeCgiPipes();
		return E_CGI_SERVERERROR;
	}
	if (this->pid_ == 0) { // child process
		this->forking_successful_ = true;

		dup2(this->pipe_into_cgi_[E_PIPE_END_WRITE], STDIN_FILENO);
		dup2(this->pipe_from_cgi_[E_PIPE_END_WRITE], STDOUT_FILENO);

		ssize_t 	bytes_sent = 0;
		ssize_t		msg_length = body_string.empty() ? 1 : body_string.length();

		if (body_string.empty())
			bytes_sent = write(STDIN_FILENO, "\0", 1);
		else
			bytes_sent = write(STDIN_FILENO, body_string.c_str(), body_string.length());
	
		this->closeCgiPipes();

		if (bytes_sent != msg_length) {
			Logger::log(E_ERROR, COLOR_RED, "not all body_string bytes were sent; aborting cgi process");	// if we get here there was an error in execve!
			deleteAllocatedCStringArray(this->args_);
			deleteAllocatedCStringArray(this->metavariables_);
			std::exit(EXIT_FAILURE);
		}
		std::cerr << "before execv" << std::endl;
		execve(this->args_[0], this->args_, this->metavariables_);

		Logger::log(E_ERROR, COLOR_RED, "execve error: %s", strerror(errno));	// if we get here there was an error in execve!
		deleteAllocatedCStringArray(this->args_);
		deleteAllocatedCStringArray(this->metavariables_);
		std::exit(EXIT_FAILURE);
	} else {	// parent process
		this->forking_successful_ = true;

		close(this->pipe_into_cgi_[E_PIPE_END_READ]);	// close all pipe ends except pipe_out_[0] (read end) as we'll use that in the storeCgiOutput_!
		close(this->pipe_into_cgi_[E_PIPE_END_WRITE]);
		close(this->pipe_from_cgi_[E_PIPE_END_WRITE]);

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
int		CgiHandler::storeCgiOutput_( void ) {

	ssize_t	ret = 1;
	ssize_t	bytesread = 0;
	char	buffer[CGI_OUTPUT_BUFFER]; // I've put the CGI_OUTPUT_BUFFER as 102 400, change if need be (I've seen this go to almost half a mil in size)

	// this->cgi_output_.clear();
	this->cgi_output_as_string_.clear();
	memset(buffer, 0, CGI_OUTPUT_BUFFER);

	while (ret > 0) {
		ret = read(this->pipe_from_cgi_[E_PIPE_END_READ], buffer, 1024); // set up a read_max (what should it even be?)

		if (ret > 0) {
			this->cgi_output_as_string_.append(buffer, ret); //make sure this isn't adding extra if buffer not full, also case for bin info in html
			bytesread += ret;
		}
		if (bytesread >= CGI_OUTPUT_BUFFER) {
			close(this->pipe_from_cgi_[E_PIPE_END_READ]);
			return E_CGI_SERVERERROR;
		}
	}

	close(this->pipe_from_cgi_[E_PIPE_END_READ]);
	std::cerr << " THE STRING: "<< cgi_output_as_string_ << std::endl;
	if (ret < 0) {
		Logger::log(E_ERROR, COLOR_RED, "storeCgiOutput read returned -1 (cannot use errno after read to find reason for failure)");
		return E_CGI_SERVERERROR;
	}

	// this->cgi_output_as_string_ = std::string(this->cgi_output_.begin(), this->cgi_output_.end());

	return E_CGI_OK;
}
