
#include "Response.hpp"
#include <sstream>

/* CONSTRUCTORS */

/*! \brief Response's default constructor that is private
*       
*  This default constructor is private.
*/
Response::Response( void ) {}

/*! \brief Constructor intializes empty response and body; sets status code to 0
*       
*  Default constructor intializes empty response and body.
*  Zeroes all attributes on construction.
*/
Response::Response( Server* server )
: response_(""), 
body_(""),  
response_mime_(""), 
resource_path_(""),
alias_location_(""), 
resource_location_(""), 
status_code_(E_UNSET), 
server_(server), 
request_(NULL),
redirect_(false),
alias_(false),
directory_listing_(false),
query_string_(""),
temp_filepath_(""), 
temp_file_(false) {

}

/*! \brief Copy constructor calls assignment operator.
*       
*/
Response::Response( const Response& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief Default destructor has no special actions.
*       
*/
Response::~Response( void ) {} 

/* OPERATOR OVERLOADS */

/*! \brief Assignment operator copies all non-static values.
*       
*
*   @param rhs a reference to the Response object whose values are to be copied
* 	@return @b Response& the response object that copies the values (left-hand side)
*/
Response&	Response::operator=( const Response& rhs ) {

	if (this != &rhs) {

		this->response_ = rhs.response_;
		this->body_ = rhs.body_;
		this->response_mime_ = rhs.response_mime_;
		this->resource_path_ = rhs.resource_path_;
		this->alias_location_ = rhs.alias_location_;
		this->resource_location_ = rhs.resource_location_;
		this->status_code_ = rhs.status_code_;
		this->server_ = rhs.server_;
		this->request_ = rhs.request_;
		this->redirect_ = rhs.redirect_;
		this->alias_ = rhs.alias_;
		this->directory_listing_ = rhs.directory_listing_;
		this->query_string_ = rhs.query_string_;
		this->temp_filepath_ = rhs.temp_filepath_;
		this->temp_file_ = rhs.temp_file_;
	}
	return *this;
}

/* CLASS PUBLIC METHODS */


/*! \brief phase 1 of response checks if request is valid and calls the function for 
*				the method requested given a Request object
*
*	The request is checked against its corresponding server's settings. If the request is allowed
*	and the resource is available, the method is called. Within the methods,
*	if it's a cgi method, it will be handled by the CgiHandler.
*
* 	@param request a pointer to the Request object that the response is created for
*/
void	Response::createResponsePhase1( Request* request ) {

	std::string			possible_methods[] ={ "GET", "HEAD", "POST", "DELETE"};
	response_methods_	methods = { &Response::getMethod_, &Response::headMethod_, &Response::postMethod_, &Response::deleteMethod_ };

	this->request_ = request;
	if (this->server_ == NULL) {
		Logger::log(E_ERROR, COLOR_RED, "Response has no server");
		this->status_code_ = E_INTERNAL_SERVER_ERROR;
		return ;
	}
	if (this->request_ == NULL) {
		Logger::log(E_ERROR, COLOR_RED, "Response has does not have a valid request");
		this->status_code_ = E_INTERNAL_SERVER_ERROR;
		return ;
	}
	this->status_code_ = request->getStatusCode();
	if (this->status_code_ >= 400)
		return ;
	if (this->server_->getClientMaxBodySize() < static_cast<double>(request->getBodySize())) {
		this->status_code_ = E_PAYLOAD_TOO_LARGE;
		return ;
	}
	if (setResourceLocationAndName_(this->request_->getRequestLineValue("uri")) >= 400 || (this->status_code_ / 100 == 3 && this->request_->getRequestLineValue("method") != "POST")) {
		return ;
	}
	if (!methodAllowed_(this->request_->getRequestLineValue("method"))) {
		this->status_code_ = E_METHOD_NOT_ALLOWED;
		Logger::log(E_DEBUG, COLOR_CYAN, "405 Method not allowed, %s, on server %s for uri: `%s'", this->request_->getRequestLineValue("method").c_str(), this->server_->getServerName().c_str() , this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	validateResource_();
	if (this->status_code_ < 400 && !this->request_->getComplete()) {
		if (this->request_->checkRequestTimeout() == true) {
			this->status_code_ = E_REQUEST_TIMEOUT;
		}
		else if (this->request_->getChunked()) {
			this->status_code_ = E_CONTINUE;
			Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Request is chunked and is not finished. 100 OK set!");
		}
		else if (!this->request_->getHeaderValueByKey("Expect").empty()) {
			this->status_code_ = E_CONTINUE;
			Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Request is not complete. Expect Header found!");
		}
		else {
			this->status_code_ = E_SERVER_PROCESSING;
			Logger::log(E_DEBUG, COLOR_CYAN, "Response Generated before request complete. 202 Accepted set.");
		}
		return ;
	}
	if (this->status_code_ < 400) {
		for (unsigned long i = 0; i < (sizeof(possible_methods)/sizeof(std::string)); i++) {
			if (this->request_->getRequestLineValue("method") == (possible_methods[i])) {
				(this->*methods[i])();
				break ;
			}
		}
	}
}


/*! \brief phase 2 of response returns a reference to the response string built
*       
*	This is the second phase (of two, first phase is Response::createResponsePhase1)
*	of creating a response to a client request.
*	Builds and returns response, checking if there has been an error, adding headers as 
*	required, and appending the body. All are separated by the CRLF or "/r/n" per HTTP 
*	guidelines.
*	
*	@return @b string& the built response string
*/
std::string&	Response::buildAndGetResponsePhase2( void ) {

	if (this->status_code_ == E_SERVER_PROCESSING) {
		return this->response_;
	}
	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400 || this->status_code_ == E_UNSET) {
		createErrorBody_();
	}
	this->response_ = addHeaders_(this->response_);
	if (!this->body_.empty()) {
		this->response_ += this->body_ + CRLF;
	}
	this->response_ += CRLF;
	return this->response_;
}

/*! \brief \brief phase 2 of response returns a reference to the response string built.
			takes a reference to the body returned from a cgi process and returns a 
*			reference to the response string built.
*       
*
*	If the cgi process has run with out error, the body from the cgi is returned as 
*	the full response. If the status code indicates an error, this will builds and returns 
*	response, adding headers as required, and appending the body passed to the function. 
*	All are separated by the CRLF or "/r/n" per HTTP guidelines.
*  
*	@param body returned from a cgi process
*	@return @b string& the built response string	
*/
std::string&	Response::buildAndGetResponsePhase2( const std::string& body ) {
	
	this->body_ = body;

	if (this->status_code_ == E_SERVER_PROCESSING) {
		return this->response_;
	}
	if (this->status_code_ < 400) {
		if (this->temp_file_)
			deleteTempFile_();
		return this->body_;
	}
	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400 || this->status_code_ == E_UNSET) {
		createErrorBody_();
	}
	this->response_ = addHeaders_(this->response_);
	if (!this->body_.empty()) {
		this->response_ += this->body_ + CRLF;
	}
	this->response_ += CRLF;
	return this->response_;
}

/*! \brief clear method resets the response for next use
*       
*
* 	Zeroes out all the attributes for the next time the response is used.
*/
void	Response::clear( void ) { 	/* reset for next use */

	this->response_ = "";
	this->body_ = "";
	this->response_mime_ = "";
	this->resource_path_ = "";
	this->resource_location_ = "";
	this->alias_location_ = "";
	this->status_code_ = E_UNSET;
	this->request_ = NULL;
	this->redirect_ = false;
	this->alias_ = false;
	this->directory_listing_ = false;
	this->query_string_ = "";
	this->temp_filepath_ = "";
	this->temp_file_ = false;
}

/******************************* SETTERS ******************************/

/*! \brief setter for the response status code
*
*
*	Allows for client and cgi handler to set the response status code.
*	@param new_code a new status code for the response
*/
void	Response::setStatusCode( unsigned int new_code ) {

	this->status_code_ = new_code;
}

/*! \brief setter for the response's server pointer attribute
 * 
 * @param server pointer to the server that is to be set for the response
 */
void	Response::setServer( Server* server ) {

	if (server) {
		this->server_ = server;
	}
}

/******************************* GETTERS ******************************/

/*! \brief	returns http status code set for the response
*
*	@return @b int http status code set for the response
*/
int	Response::getStatusCode( void ) const {

	return this->status_code_;
}


/*! \brief returns the filepath to the requested resource
*
*	@return @b const @b std::string& which is filepath to the requested resource
*/
const std::string&	Response::getResourcePath( void ) const {

	return this->resource_path_;
}

/*! \brief returns const reference to the query string for a CGI script
*
*	@return @b const @b std::string& to the query string for a CGI script
*/
const std::string&	Response::getQueryString( void ) const {

	return this->query_string_;
}

/*! \brief returns const reference to the stored filedata string meant for upload
*
*	@return @b const @b std::string& of the stored filedata meant for upload
*/
const std::string&	Response::getUploadData( void ) {

	return this->request_->getUploadContent();
}

/* CLASS PRIVATE METHODS */

/*! \brief checks for existance and file access rights for requested resource
*
*	Checks for existance and access rights for requested resource.
*	Error codes may be set, 404 - not found or 403 - access not allowed.
*/
void	Response::validateResource_( void ) {

	if (access(this->resource_path_.c_str(), F_OK) != 0) {
		this->status_code_ = E_NOT_FOUND;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found validating resource exists: `%s'", this->resource_path_.c_str());
		return;
	}
	if (!this->request_->getCgiFlag() && this->request_->getRequestLineValue("method") == "DELETE") {
		return;
	}
	else if (this->request_->getCgiFlag() && access(this->resource_path_.c_str(), X_OK) != 0) {
		this->status_code_ = E_FORBIDDEN;
		Logger::log(E_DEBUG, COLOR_CYAN, "403 execution access not allowed for cgi file: `%s'", this->request_->getRequestLineValue("uri").c_str());
		return;
	}
	else if (access(this->resource_path_.c_str(), R_OK) != 0) {
		this->status_code_ = E_FORBIDDEN;
		Logger::log(E_DEBUG, COLOR_CYAN, "403 read access not allowed for resource file: `%s'", this->request_->getRequestLineValue("uri").c_str());
	}
}

/****************************************** HEADER GENERATORS ******************************************/

/*! \brief	calls methods to add headers to the response
*
*	Appends each header to the response as needed. No errors are set here. 
*	Returns a reference to the response string. All are separated by the CRLF or "/r/n" 
*	per HTTP guidelines.
*	
*	@param response the response string
*	@return @b std::string& the response with the headers added
*/
std::string&	Response::addHeaders_( std::string& response) const {

	response += this->timeStampHeader_() + CRLF;
	response += this->contentLengthHeader_() + CRLF;
	response += this->contentLocationHeader_() + CRLF;
	if (this->body_.length() != 0)
		response += this->contentTypeHeader_() + CRLF;
	if (this->redirect_) {
		response += this->locationHeader_() + CRLF;
	}
	if (this->status_code_ == E_PAYLOAD_TOO_LARGE) {
		response += this->retryAfterHeader_() + CRLF;
	}
	if (this->status_code_ == E_REQUEST_TIMEOUT) {
		response += this->connectionHeader_(false) + CRLF;
	}
	response += CRLF;
	return ( response );
}

/*! \brief creates `Connection' header to add to response
*       
*	`Retry-After' header returned with default value in seconds for client to wait
*	before sending a request again.
*	
*	@param connection_continue a bool which determines which header value should be set
*	@return @b std::string the 'Connection' header
*/
std::string	Response::connectionHeader_( bool connection_continue ) const {

	if (!connection_continue) 
		return "Connection: close";
	else
		return "Connection: keep-alive";
}

/*! \brief creates `Retry-After' header to add to response
*       
*	`Retry-After' header returned with default value in seconds for client to wait
*	before sending a request again.
*
*	@return @b std::string the 'Retry-Header'  
*/
std::string	Response::retryAfterHeader_( void ) const {

	return "Retry-After: 30";
}

/*! \brief creates the location header for a redirection, sets redirect the index page of 
*				redirect location found
*	
*	Creates `Location' header to instruct client that resources requested are at indicated
*	location. If no index is found for the location, index is still returned as the location
*	of the resource. 
*
*	NOTES:
*	-  Index value is guaranteed to be set by the validator.
*
*	@return @b std::string the 'Location' header
*/
std::string	Response::locationHeader_( void ) const {

	std::string	redirect_path = this->resource_path_;
	std::string	path_location;
	size_t	location_pos;
	
	this->alias_ ? path_location = this->alias_location_ : path_location = this->resource_location_;
	if (path_location == "/") {
		path_location = this->server_->getRoot();
	}
	location_pos = redirect_path.find(path_location);
	if (location_pos == std::string::npos) {
		return "Location: " + this->resource_location_ + "/" + "index.html";
	}
	else {
		redirect_path = redirect_path.substr(location_pos + path_location.length());
		if (this->resource_location_ == "/")
			return "Location: " + redirect_path;
		else
			return "Location: " + this->resource_location_ + redirect_path;
	}
}

/*! \brief timeStampHeader private method returns header formated current time in GMT
*       
* 	Current time in GMT is prepended with `Date: ` in the HTTP 1.1 format.
*
* 	@return @b std::string the timeStampHeader
*/
std::string		Response::timeStampHeader_( void ) const {

	time_t	now = time(0);
	struct tm tstruct;
	std::string	time_stamp("Date: ");
	char	buffer[100];

	tstruct = *gmtime(&now);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %X GMT", &tstruct);
	time_stamp += buffer;
	return time_stamp;
}

/*! \brief returns formated response header for Content-Type header
*
*	@return @b std::string Content-Type header based on mime type set for the response
*/
std::string	Response::contentTypeHeader_( void ) const {

	return "Content-Type: " + this->response_mime_;
}

/*! \brief returns formatted response header for Content-Length
*
*	@return @b std::string formatted response header for Content-Length
*/
std::string Response::contentLengthHeader_( void ) const {
	
	return "Content-Length: "  + intToString(this->body_.length());
}

/*! \brief returns formatted response header for Content-Location
*
*
*	Returns formatted response header for Content-Location.
*	Resource_location is used always as it is overwritten in case of redirection 
*	and in case of alias the aliased location should not be sent.
*
*	@return @b std::string formatted response header for Content-Location
*/
std::string Response::contentLocationHeader_( void ) const {

	return "Content-Location: " + this->resource_location_;
}

/****************************************** GET ERROR PAGE ******************************************/

/*! \brief sets body of response to error content
*	
*	Creates the body of the error response either from a file given in the config or
*	generates a default error page from the ResponseCodes class.
*/
void	Response::createErrorBody_( void ) {
	
	std::string	str_error = "error_page_" + intToString(this->status_code_);
	bool		complete = false;

	this->body_.clear();
	if (this->server_->isErrorPage(str_error)) {
		std::string error_filename = this->server_->getErrorPage(str_error);
		Logger::log(E_DEBUG, COLOR_CYAN, "Error file found in config for : %d, filename: %s", this->status_code_, error_filename.c_str());
		if (access(error_filename.c_str(), F_OK) == 0 && access(error_filename.c_str(), R_OK) == 0) {
			std::ifstream file(error_filename);
			if (!file.is_open() || file.fail() || file.bad()) {
				Logger::log(E_DEBUG, COLOR_CYAN, "Error opening error page file : %d", this->status_code_);
			}
			else {
				std::string line;
				while (std::getline(file, line, '\n')) {
				if (!file.eof())
					line += "\n";
				this->body_ += line;
				}
				file.close();
				complete = true;
			}
		}
	}
	if (!complete) {
		this->body_ = ResponseCodes::getCodeElementBody(this->status_code_);
	}
	this->response_mime_ = MimeTypes::getMimeTypeByExtension("html");
}

/****************************************** SHARED CHECKS BEFORE METHOD ******************************************/

/*! \brief changes location if redirection found
*		if `return' key is set in the original location from the uri.
*
*	NOTES:
*	- Handles a single redirection; if multiple are chained together, the client
*	must send a request for each.
*  
*	@return @b bool indicating if the location should be changed or not
*/
bool	Response::handleRedirection_( void ) {

	if (this->server_->isKeyInLocation(this->resource_location_, "return")) {
	    this->resource_location_ = this->server_->getLocationValue(this->resource_location_, "return")->front();
		this->redirect_ = true;
		this->status_code_ = E_FOUND;
		return true;
	}
	return false;
}

/*! \brief checks for location alias and sets relevant variables
*	
*	If `alias' key is set in the original location from the uri, this function
*	will sets alias bool to true. Will also check to see if the alias points
*	to the /cgi-bin location, if so then sets flag in request.
*/
void	Response::handelAlias_( void ) {

	if (this->server_->isKeyInLocation(this->resource_location_, "alias")) {
	    this->alias_location_ = this->server_->getLocationValue(this->resource_location_, "alias")->front();
		this->alias_ = true;
	    Logger::log(E_DEBUG, COLOR_CYAN, "Alias found : location => %s, alias_location => %s", this->resource_location_.c_str(), this->alias_location_.c_str());
		if (this->alias_location_ == "/cgi-bin")
			this->request_->setCgiFlag(true);
	}
}

/*! \brief sets the location name in the server that relates to the request uri
*	
*    setResourceLocation_:
*		-  checks if the request is for cgi, rejects as invalid for not specifying a specific script
*		-  check if location is valid
*		-  calls `handlRedirection' to change location as needed
*	Error codes:
*		404 - Not Found if location does not exist
*		400 - Invalid Request if no cgi script is defined
*
*	@param uri string of the uri
*	@param is_dir bool that tells if it is directory or not
*	@param last_slash_pos size_t of the last slash's position in uri string
*/
void	Response::setResourceLocation_( std::string& uri, bool is_dir, size_t last_slash_pos ) {

	if (is_dir) {
		this->resource_location_ = uri; //won't have root applied
		if (this->request_->getCgiFlag()) {
			this->status_code_ = E_NOT_ACCEPTABLE;
			Logger::log(E_DEBUG, COLOR_CYAN, "406 CGI script not given by request: `%s'", uri.c_str());
		}
	}
	else {
		this->resource_location_ = uri.substr(0, last_slash_pos); //path only, first part of uri
		if (this->resource_location_.empty()) {
			this->resource_location_ = "/";
		}
	}
	if (!this->server_->isLocationInServer(this->resource_location_)) {
		this->status_code_ = E_NOT_FOUND;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found while setting location and name for resource: `%s'", uri.c_str());
		return ;
	}
	if (!handleRedirection_())
		handelAlias_();
}

/*! \brief sets resource path based on verified location from the uri
*	
*    setResourcePath_:
*		-  sets resource path as path to index of this page
*		-  verifies that a cgi script is listed in the server config
*       -  checks for autoindex and sets directory-listing `true` if found
*	Error codes:
*		404 - Not Found if location does not exist
*		400 - Invalid Request if no cgi script is defined
*  
*	@param uri string of the uri
*	@param is_dir bool that tells if it is directory or not
*	@param last_slash_pos size_t of the last slash's position in uri string
*/
void	Response::setResourcePath_( std::string& uri, bool is_dir, size_t last_slash_pos ) {

	if (is_dir) {
		if ((this->server_->isKeyInLocation(this->resource_location_, "autoindex")
		&& this->server_->getLocationValue(this->resource_location_, "autoindex")->front() == "on")
		|| (this->alias_ && this->server_->isKeyInLocation(this->alias_location_, "autoindex")
		&& this->server_->getLocationValue(this->alias_location_, "autoindex")->front() == "on")){
			this->directory_listing_ = true;
			this->alias_ ? this->resource_path_ = this->server_->getLocationValue(this->alias_location_, "root")->front() 
			: this->resource_path_ = this->server_->getLocationValue(this->resource_location_, "root")->front();
		}
		else {
			if (this->alias_ && !this->server_->isKeyInLocation(this->resource_location_, "index"))
				this->resource_path_ = this->server_->getLocationValue(this->alias_location_, "index")->front();
			else
				this->resource_path_ = this->server_->getLocationValue(this->resource_location_, "index")->front();
			this->redirect_ = true;
			this->status_code_ = E_MOVED_PERMANENTLY;
		}
	}
	else {
		std::string	filename = uri.substr(last_slash_pos + 1);
		std::string location;
		
		this->alias_ ? location = this->alias_location_ : location = this->resource_location_;
		if (!this->request_->getCgiFlag()) {
			//make case for root here so no extra / is added if location == /
			this->resource_path_ =  this->server_->getRoot() + location + "/" + filename;
		}
		else if (this->request_->getCgiFlag() && !this->server_->isScriptOnCgiList(filename)) {
			this->status_code_ = E_NOT_FOUND;
			Logger::log(E_DEBUG, COLOR_CYAN, "404 CGI script given by request was not on approved list: `%s'", uri.c_str());
		}
		else if (this->request_->getCgiFlag()) {
			this->resource_path_ = (this->server_->getLocationValue("/cgi-bin", "root"))->front() + "/" + filename;
		}
	}
}

/*! \brief Extracts information from the request URI and validates 
*				the location requested
*       
* 	Check for the validity of the location and possible CGI information in 
*	the URI form the request. The existence of the files is not checked, only that 
*	they have been entered into the server at this point.
*	
*	404 - Not Found will be set if location does not exist.
*
*	@param uri string of the uri
*	@return @b int the status code  
*/
int	Response::setResourceLocationAndName_( std::string uri ) {
	
	size_t	last_slash_pos = uri.find_last_of('/');

	if (last_slash_pos != std::string::npos && (last_slash_pos != uri.length() || uri == "/")) {
		std::string path;
		bool		is_dir;

		uri == "/" ? path = this->server_->getRoot() : path = this->server_->getRoot() + uri;
		is_dir = isDirectory(path) || this->server_->isLocationInServer(uri);
		setResourceLocation_(uri, is_dir, last_slash_pos);
		if (this->status_code_ < 400)
			setResourcePath_(uri, is_dir, last_slash_pos);
	}
	else {
		this->status_code_ = E_BAD_REQUEST;
		Logger::log(E_DEBUG, COLOR_CYAN, "URI is invalid : `%s'", uri.c_str());
	}
	return this->status_code_;
}

/*! \brief returns true or false if requested method is allowed for the location
*
*	Uses getLocationValue from server to get the vector of allowed methods,
*	searches them using find to see if requested method is contained in the vector.
*
*	Return : false if no methods listed or method not found and true if found.
*
*	@param method string of the method
*	@return @b bool that tells if if requested method is allowed for the location
*/
bool	Response::methodAllowed_( std::string method ) {

	const std::vector<std::string>*	methods;
	
	this->alias_ ? 
	methods = this->server_->getLocationValue(this->alias_location_, "allow_methods") 
	: methods = this->server_->getLocationValue(this->resource_location_, "allow_methods");
	
	if (!methods || methods->empty()) {
		return false;
	}
	else if (std::find(methods->begin(), methods->end(), method) == methods->end()) {
		return false;
	}
	else {
		return true;
	}
}

/************************************************* GET *************************************************/

/*! \brief	returns the accepted formats from request in a vector
*
*	Retrieves the accepted format header fromt he request and splits on `,'
*	removing references to OWS tokens as they are not currently used.
*
*	@return @b accepted_formats a vector of strings with the mimetypes of the accepted formats
*/
std::vector<std::string>	Response::getAcceptedFormats_( void ) {

	std::vector<std::string>	accepted_formats;
	std::string 				accepted_formats_from_header = this->request_->getHeaderValueByKey("Accept");
	std::string					format;

	if (!accepted_formats_from_header.empty()) {
		std::stringstream ss(accepted_formats_from_header);
		while (getline(ss, format, ',')) {
			if (format.find_first_of(';') != std::string::npos) {
				format = format.substr(0, format.find_first_of(';'));
			}
			accepted_formats.push_back(format);
		}
	}
	return accepted_formats;
}

/*! \brief sets the content type MIME for the response header
*
*	Sets the content type MIME for the response header.
*	- cgi responses have `html` set as their mime type
*	- files without extensions are sent as `unknown` which is sent as octet stream
*	- otherwise if extension was not in the mime_types_ map the response is set 
*		set to error code of 415 - no supported media type
*/
void	Response::setMimeType_( void ) {

	size_t	extension_start = this->resource_path_.find_last_of('.');

	if (this->request_->getCgiFlag() || this->directory_listing_) {
		this->response_mime_ = MimeTypes::getMimeTypeByExtension("html");
		return ;
	}
	if (extension_start == std::string::npos) {
		this->response_mime_ = MimeTypes::getMimeTypeByExtension("unknown");
	}
	else {
		std::string extension = this->resource_path_.substr(extension_start + 1);
		this->response_mime_ = MimeTypes::getMimeTypeByExtension(extension);
	}
	if (this->response_mime_.empty()) {
		this->status_code_ = E_UNSUPPORTED_MEDIA_TYPE;
		Logger::log(E_DEBUG, COLOR_CYAN, "415 File extension not supported media type: `%s'", this->request_->getRequestLineValue("uri").c_str());
	}
}

/*! \brief	implements the GET method
*
*	Implements the http GET method. Checks the resource against the accepted formats in the request.
*	Sets the MIME type for the response.
*	Will call the method to build the appropriate body.
*
*	In case of CGI, no body.
*	In case of directory listing, builds list of linked files in body.
*	Otherwise will call buildBody_ to copy directly from file.
*/
void	Response::getMethod_( void ) {

	std::vector<std::string>	accepted_formats = getAcceptedFormats_();

	setMimeType_();
	if (this->status_code_ >= 400) {
		return ;
	}
	if (accepted_formats.empty() || std::count(accepted_formats.begin(), accepted_formats.end(), "*/*") || std::count(accepted_formats.begin(), accepted_formats.end(), this->response_mime_)) {
		
		if (this->request_->getCgiFlag()) {
			Logger::log(E_DEBUG, COLOR_CYAN, "getMethod_ cgiFlag true");
			return ;
		}
		else if (this->directory_listing_ == true){
			this->body_ = buildHtmlList(this->resource_path_);
		}
		else if (this->response_mime_.compare(0, 4, "text") == 0) {
			buildBody_(this->resource_path_, std::ifstream::in);
		}
		else {
			buildBody_(this->resource_path_, std::ifstream::binary);
		}
		if (this->status_code_ < 400) {
			this->status_code_ = E_OK;
		}
	}
	else {
		this->status_code_ = E_NOT_ACCEPTABLE;
	}
}

/*! \brief saves body content to send in the body attribute
*
*	Opens file and copies the string to the body.
*	If it's a text file or a binary file, it is handled differently.
*
*	@param path path to the file
*	@param mode the ios_base open mode indicating if the file should be opened for binary or text file
*/
void	Response::buildBody_( std::string& path, std::ios_base::openmode mode ) {

	std::ifstream	resource(path, mode);
	
	if (!resource.is_open() || resource.fail() || resource.bad()) {
		this->status_code_ = E_FAILED_DEPENDENCY;
		Logger::log(E_DEBUG, COLOR_CYAN, "424 cannot open resource: %s.", this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	if (mode == std::ios::binary) {
		std::stringstream		contents;
		contents << resource.rdbuf();
		this->body_ += contents.str();
	}
	else {
		std::string line;
		while (std::getline(resource, line, '\n')) {
			if (!resource.eof())
				line += "\n";
			this->body_ += line;
		}
	}
	resource.close();
}

/****************************************** HEAD ******************************************/

/*! \brief	implements HEAD method, returns 200 if resource acceptable to client
*				and 415 if not.
*
*	Checks the MIME type of the requested resource against the acceptable formats
*	from client. If MIME is unsuported or unacceptable, the error status code 
*	is set. Else 200 OK status code is set.
*/
void	Response::headMethod_( void ) {

	std::vector<std::string>	accepted_formats = getAcceptedFormats_();

	setMimeType_();
	if (this->status_code_ >= 400) {
		return ;
	}
	if (accepted_formats.empty() || std::count(accepted_formats.begin(), accepted_formats.end(), "*/*") || std::count(accepted_formats.begin(), accepted_formats.end(), this->response_mime_)) {
		if (this->status_code_ < 400) {
			this->status_code_ = E_OK;
		}
	}
	else {
		this->status_code_ = E_NOT_ACCEPTABLE;
		Logger::log(E_DEBUG, COLOR_CYAN, "HEAD: 406 MIME type of resource not in client `Accept` list: %s.", this->request_->getRequestLineValue("uri").c_str());
	}
}

/****************************************** DELETE ******************************************/

/*! \brief if no cgi present, calls remove on resource path to delete file, otherwise sets 
*				query string to processed body
*
*	If no cgi present, calls remove() on resource path to delete file, otherwise sets
*	query string to processed body. In case of failure 500, server error is set.
*	The query string will be url encoded if the query_encode_ from the request is true.
*/
void	Response::deleteMethod_( void ) {

	if (this->request_->getCgiFlag()) {
		if (this->request_->getQueryEncode())
			this->query_string_ = urlEncode(this->request_->getProcessedBody());
		else
			this->query_string_ = this->request_->getProcessedBody();
	}
	else if (std::remove(this->resource_path_.c_str()) != 0 ) {
		Logger::log(E_ERROR, COLOR_RED, "DELETE: removal of resource failed : `%s'", this->request_->getRequestLineValue("uri").c_str());
		this->status_code_ = E_FAILED_DEPENDENCY;
	}
	else {
		this->status_code_ = E_OK;
	}
}

/****************************************** POST ******************************************/

/*! \brief creates resource requested for non-cgi requests
*       
*	Creates file for request based on multipart/form-data information parsed in request.
*	File is created based on the uri location. If a file already exists by that name
*	it will be overwritten.
*	If the filename is not given, no resource wil be created and a bad request will be flagged.
*	if a save directory was set in the config file, the resource will be saved there.
*	If successful, the 201 created status code is set.
*  
*	@param is_save_dir bool indicating if the location has an upload_store configured
*/
void	Response::saveBodyToFile_( bool is_save_dir ) {

	const std::string& filename = this->request_->getUploadName();
	std::string file_path;

	if (filename.empty()) {
		this->status_code_ =  E_BAD_REQUEST;
		return;
	}
	if (is_save_dir) {
		file_path = this->server_->getLocationValue(this->resource_location_, "save_dir")->front() + "/" + filename;
	}
	else {
		file_path = this->server_->getLocationValue(this->resource_location_, "root")->front() + "/" + filename;
	}
	std::ofstream	new_file(file_path, std::ostream::binary);
	if (new_file.bad() || new_file.fail() || !new_file.is_open()) {
		Logger::log(E_ERROR, COLOR_RED, "POST: opening failed : `%s'", this->request_->getRequestLineValue("uri").c_str());
		this->status_code_ = E_FAILED_DEPENDENCY;
		return ;
	}
	new_file << this->request_->getUploadContent();
	new_file.close();
	this->status_code_ = E_CREATED;
}

/*! \brief	creates a temporary file in the server upload_store and saves upload data there.
*
*	Get's the filename from request. Will attempt to create file with that name
*	and save upload data in there.	
*/
void	Response::saveBodyToTempFile_( void ) {

	const std::string& filename = this->request_->getUploadName();
	std::string file_path;

	if (filename.empty()) {
		this->status_code_ =  E_BAD_REQUEST;
		return;
	}
	else {
		file_path = this->server_->getUploadStore() + "/" + filename;
	}
	std::ofstream	new_file(file_path, std::ostream::binary);
	if (new_file.bad() || new_file.fail() || !new_file.is_open()) {
		this->status_code_ = E_FAILED_DEPENDENCY;
		Logger::log(E_ERROR, COLOR_RED, "Temporary file creation failed : %s", file_path.c_str());
		return ;
	}
	new_file << this->request_->getUploadContent();
	new_file.close();
	this->request_->clearUploadContent();
	this->temp_file_ = true;
	this->temp_filepath_ = file_path;
}

/*! \brief	post method will create resource or set query string for cgi script
*
*	The query string will be url encoded if the query_encode_ from the request is true.
*/
void	Response::postMethod_( void ) {

	std::string	content_type_values = this->request_->getHeaderValueByKey("Content-Type");
	bool		cgi_flag = this->request_->getCgiFlag();
	
	if (content_type_values.empty() || content_type_values.find("form") == std::string::npos) {
		this->status_code_ = E_BAD_REQUEST;
		return ;
	}
	else if (!cgi_flag) {
		this->saveBodyToFile_(this->server_->isKeyInLocation(this->resource_location_, "save_dir"));
	}
	else {
		setMimeType_();
		if (this->request_->isFileUpload()) {
			if (this->request_->getUploadContent().size() > FILE_SIZE_LIMIT_FOR_PIPE)
				this->saveBodyToTempFile_();
		}
		this->query_string_ = this->request_->getProcessedBody();
		if (this->temp_file_)
			this->query_string_ += "temp_filepath=" + this->temp_filepath_;
		if (this->request_->getQueryEncode())
			this->query_string_ = urlEncode(this->query_string_) ;
	}
}

/*! \brief	deletes temporary file used for cgi file uploads
*
*	deletes temporary file used for cgi file uploads.
*/
void	Response::deleteTempFile_( void ) {

	if (std::remove(this->temp_filepath_.c_str()) != 0 ) {
		Logger::log(E_ERROR, COLOR_RED, "Removal of temporary file failed : `%s'", this->temp_filepath_.c_str());
	}
}