
#include "CgiHandler.hpp"


CgiHandler::CgiHandler( void )
	:	metavariables_(NULL),
		args_(NULL),
		path_(NULL),
		piping_successful_(false),
		forking_successful_(false),
		pipe_in_({-1, -1}),
		pipe_out_({-1, -1}),
		pid_(-1) {}

CgiHandler::CgiHandler( const CgiHandler& to_copy ) {

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

void	CgiHandler::ClearCgiHandler( void ) {
	
	this->metavariables_map_.clear();
	this->cgi_map_.clear();
	deleteAllocatedCStringArray(this->metavariables_);
	this->metavariables_ = NULL;
	deleteAllocatedCStringArray(this->args_);
	this->args_ = NULL;
	if (this->path_ != NULL)
		delete [] this->path_;
	this->path_ = NULL;
	this->piping_successful_ = false;
	this->forking_successful_ = false;
	for (int i = 0; i < 2; ++i) {
		this->pipe_in_[i] = -1;
		this->pipe_out_[i] = -1;
	}
	this->pid_ = -1;
}

int	CgiHandler::SELECT_initializeCgi( Client& client, ServerManager& server_manager ) {

	Request& request = client.getRequest();

	/* Request has already checked that cgi-location is valid,
	that the server has the permission,
	that you can access the cgi script/program etc. */

	int	result = this->fillMetavariablesMap_(client);

	if (result != E_CGI_OK) {

	}

}

int	CgiHandler::POLL_initializeCgi( Client& client, ServerManager& server_manager ) {

	Request& request = client.getRequest();

	/* Request has already checked that cgi-location is valid,
	that the server has the permission,
	that you can access the cgi script/program etc. */

	int	result = this->fillMetavariablesMap_(client);

	if (result != E_CGI_OK) {

	}
	this->metavariables_ = this->convertMetavariablesMapToCStringArray_();
	if (this->metavariables_ == NULL) {
		Logger::log(E_ERROR, COLOR_RED, "initializeCgi: allocation of cgi metavariables failed");
		return E_CGI_SERVERERROR;
	}



}

/* CLASS PRIVATE METHODS */


/*! \brief Fill metavariables_map_ with key/value-data. Later on the map will be converted into a
*			c-style string array which will be passed to the cgi script as the environment.
*			Returns a e_cgi_results enum value which tells if the function was successful or not.
*       
*	ASK JENNY. IS request.getHeaderValueByKey case sensitive? Is "user-agent" the same as "User-Agent"?
*/
int	CgiHandler::fillMetavariablesMap_( Client& client ) {

	Request& request = client.getRequest();

	this->metavariables_map_["AUTH_TYPE"] = "Basic";	// check this later ("Digest" and "Form" are other common values)

	this->metavariables_map_["HTTP_USER_AGENT"] = request.getHeaderValueByKey("user-agent");
	this->metavariables_map_["HTTP_COOKIE"] = request.getHeaderValueByKey("cookie");
	this->metavariables_map_["HTTP_REFERER"] = request.getHeaderValueByKey("referer");
	this->metavariables_map_["GATEWAY_INTERFACE"] = "CGI/1.1";

	this->metavariables_map_["SERVER_NAME"] = client.getServer()->getServerName();
	this->metavariables_map_["SERVER_PORT"] = client.getServer()->getListeningPortInt();
	this->metavariables_map_["SERVER_PROTOCOL"] = "http/1.1";
	this->metavariables_map_["SERVER_SOFTWARE"] = ""; // come up with server name (JAS would be great!)

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

int	CgiHandler::createCgiArguments_( Request& request ) {

}