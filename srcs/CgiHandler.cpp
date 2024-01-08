
#include "CgiHandler.hpp"

#include <signal.h>

/*! \brief Construct a new Cgi Handler:: Cgi Handler object.
 * 
 */
CgiHandler::CgiHandler( void )
	:	cgi_output_as_string_(""),
		metavariables_(NULL),
		args_(NULL),
		path_(NULL),
		pid_(-1)
{
	this->pipe_into_cgi_[0] = -1;
	this->pipe_into_cgi_[1] = -1;
	this->pipe_from_cgi_[0] = -1;
	this->pipe_from_cgi_[1] = -1;
}

/*! \brief Copy construct a new Cgi Handler:: Cgi Handler object.
 * 
 *	@param to_copy another CgiHandler object.
 */
CgiHandler::CgiHandler( const CgiHandler& to_copy )
	:	cgi_output_as_string_(""),
		metavariables_(NULL),
		args_(NULL),
		path_(NULL),
		pid_(-1)
{

	*this = to_copy;
} 

/*! \brief Destroy the Cgi Handler:: Cgi Handler object.
 * 
 *	Close all possibly open pipes and free everything allocated before final destruction.
 */
CgiHandler::~CgiHandler( void ) {

	this->closeCgiPipes();
	deleteAllocatedCStringArray(this->metavariables_);
	deleteAllocatedCStringArray(this->args_);
	if (this->path_ != NULL)
		delete [] this->path_;
} 

/* OPERATOR OVERLOADS */

/*! \brief Operator overload for '=' for CgiHandler.
 * 
 *	@param rhs the CgiHandler object whose values are to be copied.
 *	@return CgiHandler& a reference to the CgiHandler object that copied the values of the rhs.
 */
CgiHandler&	CgiHandler::operator=( const CgiHandler& rhs ) {

	if (this != &rhs) {
		this->metavariables_map_ = rhs.metavariables_map_;
		// this->cgi_map_ = rhs.cgi_map_;

		if (rhs.metavariables_ != NULL)
			this->metavariables_ = copyCStringArray(rhs.metavariables_);
		else
			this->metavariables_ = NULL;

		if (rhs.args_ != NULL)
			this->args_ = copyCStringArray(rhs.args_);
		else
			this->args_ = NULL;
		if (rhs.path_ != NULL)
			this->path_ = ft_strdup(rhs.path_);
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
 *		'Zeros' out the values of the CgiHandler.
 *       
 *	This function does not clear the cgi_output_ (that happens in the beginning of CgiHandler::StoreCgiOutput_).
 */
void	CgiHandler::ClearCgiHandler( void ) {
	
	this->metavariables_map_.clear();
	// this->cgi_map_.clear();
	deleteAllocatedCStringArray(this->metavariables_);
	this->metavariables_ = NULL;
	deleteAllocatedCStringArray(this->args_);
	this->args_ = NULL;
	this->path_ = NULL;
	this->closeCgiPipes();
	for (int i = 0; i < 2; ++i) {
		this->pipe_into_cgi_[i] = -1;
		this->pipe_from_cgi_[i] = -1;
	}
	this->pid_ = -1;
}

/*! \brief Close all pipes used for CGI.
*	
*
*/
void	CgiHandler::closeCgiPipes( void ) {

	close (this->pipe_into_cgi_[0]);
	close (this->pipe_into_cgi_[1]);
	close (this->pipe_from_cgi_[0]);
	close (this->pipe_from_cgi_[1]);
}

/*! \brief This function handles the initialization of the CGI when a cgi request has been received.
 * 		This is done in several steps and if one of these steps fails, the cgi initialization will be aborted.
 *	
 *	First the function saves the cgi and object stores the cgi resource path in the class attribute path_.
 *	After that the metavariables will be created and converted into a c-style string array.
 *	Then the arguments for the execve call will be created.
 *	Finally the pipes for the cgi process will be created.
 * 
 *	It is important to note that the Request has already checked that the cgi-location is valid,
 *	that the server has permission, that you can access the cgi sript etc.
 *
 *	@param client the client that has this CgiHandler object as an attribute.
 *	@return int which tells the result of the attempted initialization.
 */
int	CgiHandler::initializeCgi( Client& client ) {

	std::string uri = client.getRequest().getRequestLineValue("uri");

	std::string temp = client.getResponse().getResourcePath();
	this->path_ = const_cast<char*>(temp.erase(2, 8).c_str());

	int 	result;

	if ((result = this->fillMetavariablesMap_(client)) != EXIT_SUCCESS) {
		this->ClearCgiHandler();
		return result;
	}
	this->metavariables_ = this->convertMetavariablesMapToCStringArray_();
	if (this->metavariables_ == NULL) {
		this->ClearCgiHandler();
		return E_INSUFFICIENT_STORAGE;
	}
	if ((result = this->createCgiArguments_(uri, client)) != EXIT_SUCCESS) {
		this->ClearCgiHandler();
		return result;
	}
	if ((result = this->setUpCgiPipes_()) != EXIT_SUCCESS) {
		this->ClearCgiHandler();
	}
	return result;
}

/*!	\brief this function handles the actual execution of the cgi-script and storing of its output which will
 *		then be passed to the client.
 *
 * 	
 * 
 *	@param response a reference to the client's Response object.
 *	@return int which tells if this function was successful or not.
 */
int	CgiHandler::cgiFinish( Response& response ) {

	// Logger::log(E_DEBUG, COLOR_GREEN, "INSIDE cgiFinish!!!");
	int	result;

	if ((result = this->executeCgi_(response.getUploadData())) != EXIT_SUCCESS) {
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
 *		c-style string array which will be passed to the cgi script as the environment.
 * 
 *	@param client the client that has this CgiHandler object as an attribute.
 *	@return int which tells if the function was successful or not.
 */
int	CgiHandler::fillMetavariablesMap_( Client& client ) {

	Request&	request = client.getRequest();
	Server&		server = *(client.getServer());

	this->metavariables_map_["AUTH_TYPE"] = "Basic";

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
	this->metavariables_map_["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->metavariables_map_["SERVER_SOFTWARE"] = "JAS-webserver/0.75";

	this->metavariables_map_["REMOTE_HOST"] = client.getClientHost();
	this->metavariables_map_["REMOTE_PORT"] = ntohs(client.getAddress().sin_port);

	return EXIT_SUCCESS;
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
 *
 *	@return string_array which is the metavariables that will be passed to the execve as the environment variable or NULL in case of error.
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

/*! \brief get the extension of the cgi-script.
 *		Needed for creating the arguments for the execution of the cgi-script.
 * 
 *	@param uri the uri of the of the requested cgi-script.
 *	@return std::string of the cgi-script's extension
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

/*! \brief get the cgi output
 * 
 * @return const std::string& ie. the cgi output 
 */
const std::string&	CgiHandler::getCgiOutputAsString( void ) const {

	return this->cgi_output_as_string_;
}

/*! \brief get the ends of the pipe that leads INTO OF the cgi-process.
 * 
 *	@return const int* pointers to the pipe ends.
 */
const int*	CgiHandler::getPipeIn( void ) const {

	return this->pipe_into_cgi_;
}

/*! \brief get the ends of the pipe that leads OUT OF the cgi-process.
 * 
 *	@return const int* pointers to the pipe ends.
 */
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
 *
 * @param uri the uri of the of the requested cgi-script.
 * @param client the client that has this CgiHandler object as an attribute.
 * @return int which tells if this function was successful or not.
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
		return E_INSUFFICIENT_STORAGE;
	}
	if (size == 1){
		this->args_[0] = ft_strdup(this->path_);
		if (this->args_[0] == NULL)
			goto cleanup;
	}
	else {
		this->args_[0] = ft_strdup(client.getServer()->getCgiExecutor(extension).c_str());	//extension executable, for example "/bin/bash" or "/usr/local/bin/python3"
		this->args_[1] = ft_strdup(this->path_);
		if (this->args_[0] == NULL || this->args_[1] == NULL)
			goto cleanup;
	}
	return EXIT_SUCCESS;
cleanup:
	deleteAllocatedCStringArray(this->args_);
	this->args_ = NULL;
	return E_FAILED_DEPENDENCY;
}

/*! \brief This function monitors that the cgi process ends in a timely manner.
 *		If the cgi process has not ended in by the CGI_TIMEOUT the process will be
 *		sent a signal to kill the process.
 * 
 *	@param status a reference to the integer in the executeCgi_ function where the result of the
 *	cgi process is stored in.
 */
int	CgiHandler::cgiTimer_( int& status ) {

	time_t	start = time(NULL);
	int		result;

	while (difftime(time(NULL), start) <= CGI_TIMEOUT) {
		result = waitpid(this->pid_, &status, WNOHANG);
		if (result > 0)
			return EXIT_SUCCESS;
	}

	kill(this->pid_, SIGKILL);	// if it got here the CGI process has gone on for too long, close it remotely
	waitpid(this->pid_, &status, 0);
	return E_GATEWAY_TIMEOUT;
}

/*! \brief This function handles the setting up of the pipes used for both passing values for the cgi process
 *		as well as getting the results of the cgi process.
 *
 * 	First the pipes are created. Then the pipe ends that are used by the ServerManager as pollfds will be made non-blocking.
 * 
 * @return int which tells if the function was successful or not.
 */
int	CgiHandler::setUpCgiPipes_( void ) {

	if (pipe(pipe_into_cgi_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		return E_FAILED_DEPENDENCY;
	}
	if (pipe(pipe_from_cgi_) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes: %s", strerror(errno));
		this->closeCgiPipes();
		return E_FAILED_DEPENDENCY;
	}
	if (fcntl(this->pipe_into_cgi_[E_PIPE_END_WRITE], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 || fcntl(this->pipe_from_cgi_[E_PIPE_END_READ], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {	// check FD_CLOEXEC later...
		Logger::log(E_ERROR, COLOR_RED, "setUpCgiPipes fcntl error: %s", strerror(errno));
		this->closeCgiPipes();
		return E_FAILED_DEPENDENCY;
	}

	return EXIT_SUCCESS;
}

/*! \brief This function handles the execution of the cgi-script.
 *		Send body-info to pipe, fork a process and execute cgi script.
 *
 *	In the beginning of the function we fork.
 *	
 *	IN THE CHILD PROCESS we dup2 the pipe_ends used by the cgi process.
 *	After that we write the body_string into the child process's STD_IN.
 *	Then we close all the cgi pipes. Then we chdir into the cgi-bin.
 *	Finally we execve the cgi-script.
 *
 *	IN THE PARENT PROCESS we close all the pipe ends except the pipe_from_cgi_[READ_END] which we use in CgiHandler::storeCgiOutput_.
 *	Then we get the status the cgi (child) process with the CgiHandler::cgiTimer_ and if the process hangs too long or it otherwise fails
 *	(we get another exit code than 0) we mark it as a server error.
 * 
 *	@param body_string the string that will be passed to the cgi process as STD_IN.
 *	@result int which tells if function was successful or not.
 */
int		CgiHandler::executeCgi_( const std::string& body_string ) {

	// Logger::log(E_DEBUG, COLOR_GREEN, "BEFOR FORKING executeCgi!!!");
	if ((this->pid_ = fork()) == -1) {
		Logger::log(E_ERROR, COLOR_RED, "fork failure: %s", strerror(errno));
		this->closeCgiPipes();
		return E_FAILED_DEPENDENCY;
	}
	if (this->pid_ == 0) {

		dup2(this->pipe_into_cgi_[E_PIPE_END_READ], STDIN_FILENO);
		dup2(this->pipe_from_cgi_[E_PIPE_END_WRITE], STDOUT_FILENO);

		ssize_t 	total_bytes_sent = 0;
		ssize_t		msg_length = body_string.empty() ? 1 : body_string.size();

		if (body_string.empty())
			total_bytes_sent = write(this->pipe_into_cgi_[E_PIPE_END_WRITE], "\0", 1);
		else
			total_bytes_sent = write(this->pipe_into_cgi_[E_PIPE_END_WRITE], body_string.c_str(), body_string.size());
		
		this->closeCgiPipes();

		if (total_bytes_sent != msg_length) {
			Logger::log(E_ERROR, COLOR_RED, "not all body_string bytes were sent; aborting cgi process");
			deleteAllocatedCStringArray(this->args_);
			deleteAllocatedCStringArray(this->metavariables_);
			std::exit(EXIT_FAILURE);
		}
		chdir("./cgi-bin");
		execve(this->args_[0], this->args_, this->metavariables_);

		Logger::log(E_ERROR, COLOR_RED, "execve error: %s", strerror(errno));
		deleteAllocatedCStringArray(this->args_);
		deleteAllocatedCStringArray(this->metavariables_);
		std::exit(EXIT_FAILURE);
	} else {

		close(this->pipe_into_cgi_[E_PIPE_END_READ]);
		close(this->pipe_into_cgi_[E_PIPE_END_WRITE]);
		close(this->pipe_from_cgi_[E_PIPE_END_WRITE]);

		int status = 0;
		if (this->cgiTimer_(status) == E_GATEWAY_TIMEOUT)
			return E_GATEWAY_TIMEOUT;
		if (WIFSIGNALED(status) > 0 || WEXITSTATUS(status) != 0)
			return E_FAILED_DEPENDENCY;
	}

	return EXIT_SUCCESS;
}

/*! \brief Store cgi output into a string that will be sent to the client.
 *
 *	In the beginning of the function we clear the cgi_output_as_string_ attribute.
 *	Then we will simply keep looping the reading of the pipe end where the results
 *	of the cgi-script until everything has been read or the bytes read are over the CGI_OUTPUT_BUFFER.
 *	After a successful reading of the cgi output we close the pipe and we check that the there was no
 *	error with read (we cannot use errno to find out why it failed because the subject explicitly prohibits doing so).
 * 
 *	@result int which tells if the function was successful or not.
 */
int		CgiHandler::storeCgiOutput_( void ) {

	ssize_t	ret = 1;
	ssize_t	bytesread = 0;
	char	buffer[BUFFER_SIZE];

	this->cgi_output_as_string_.clear();
	memset(buffer, 0, BUFFER_SIZE);
	while (ret > 0) {
		ret = read(this->pipe_from_cgi_[E_PIPE_END_READ], buffer, BUFFER_SIZE - 1);

		if (ret > 0) {
			buffer[ret] = '\0';
			this->cgi_output_as_string_.append(buffer, ret);
			bytesread += ret;
		}
		if (bytesread >= CGI_OUTPUT_BUFFER) {
			close(this->pipe_from_cgi_[E_PIPE_END_READ]);
			return E_INSUFFICIENT_STORAGE;
		}
	}
	close(this->pipe_from_cgi_[E_PIPE_END_READ]);
	if (ret < 0) {
		Logger::log(E_ERROR, COLOR_RED, "storeCgiOutput read returned -1 (cannot use errno after read to find reason for failure)");
		return E_FAILED_DEPENDENCY;
	}
	return EXIT_SUCCESS;
}
