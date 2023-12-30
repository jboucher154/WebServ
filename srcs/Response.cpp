
#include "Response.hpp"
#include <sstream>

/* CONSTRUCTORS */

/*! \brief Default constructor is private.
*       
*  Default constructor is private.
*  
*/
Response::Response( void ) { 
	
	/* default constructor */
}

/*! \brief Constructor intializes empty response and body; sets status code to 0.
*       
*
*  Default constructor intializes empty response and body. Default status code set to 501 for MVP.
*  More details to be filled as project progresses.
*/
Response::Response( Server* server )
: response_(""), 
body_(""), 
binary_data_(), 
response_mime_(""), 
resource_path_(""),
alias_location_(""), 
resource_location_(""), 
status_code_(0), 
server_(server), 
request_(NULL),
redirect_(false),
alias_(false),
directory_listing_(false) {

}

/*! \brief Copy constructor calls assignment operator.
*       
*
*  More details to be filled as project progresses.
*  
*/
Response::Response( const Response& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief Default destructor has not special actions.
*       
*
*  More details to be filled as project progresses.
*  
*/
Response::~Response( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

/*! \brief Assignment operator copies all non static values.
*       
*
*   Assignment operator is now public.
*  
*/
Response&	Response::operator=( const Response& rhs ) {

	if (this != &rhs) {

		this->response_ = rhs.response_;
		this->status_code_ = rhs.status_code_;
		this->body_ = rhs.body_;
		this->binary_data_ = rhs.binary_data_;
		this->server_ = rhs.server_;
		this->request_ = rhs.request_;
		this->response_mime_ = rhs.response_mime_;
		this->resource_location_ = rhs.resource_location_;
		this->alias_location_ = rhs.alias_location_;
		this->resource_path_ = rhs.resource_path_;
		this->redirect_ = rhs.redirect_;
		this->alias_ = rhs.alias_;
		this->directory_listing_ = rhs.directory_listing_;
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */


/*! \brief phase 1 of response checks if request is possible and calls the function for 
*				the method requestsed given a Request object.
*       
*
*  
*  
*/
void	Response::createResponsePhase1( Request* request ) {

	std::string			possible_methods[] ={ "GET", "HEAD", "POST", "DELETE"};
	response_methods_	methods = { &Response::getMethod_, &Response::headMethod_, &Response::postMethod_, &Response::deleteMethod_ };

	this->request_ = request;
	if (this->server_ == NULL) {
		Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Response server is NULL");
		return ;
	}
	if (this->request_ == NULL || !this->request_->getComplete()) {
		if (this->request_ != NULL && !this->request_->getComplete()) { //&& this->request_->getChunked()
			this->status_code_ = 100;
			Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Request is chunked and is not finished. 100 OK set!");
		}
		else {
			this->status_code_ = 400; //Bad Request
			Logger::log(E_DEBUG, COLOR_CYAN, "Bad request, Response Generated before request complete.");
		}
		return ;
	}
	if (setResourceLocationAndName(this->request_->getRequestLineValue("uri")) >= 400 || this->status_code_ / 100 == 3) {
		return ;
	}
	if (!methodAllowed_(this->request_->getRequestLineValue("method"))) {
		this->status_code_ = 405;
		Logger::log(E_DEBUG, COLOR_CYAN, "405 Method not allowed, %s, on server %s for uri: `%s'", this->request_->getRequestLineValue("method").c_str(), this->server_->getServerName().c_str() , this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	validateResource_();
	if (this->status_code_ < 400) {
		for (unsigned long i = 0; i < (sizeof(possible_methods)/sizeof(std::string)); i++) {
			if (this->request_->getRequestLineValue("method") == (possible_methods[i])) {
				(this->*methods[i])();
				break ;
			}
		}
	}
}


/*! \brief returns a reference to the response string built.
*       
*
*	Builds and returns response, checking if there has been an error, adding headers as 
*	required, and appending the body. All are separated by the CRLF or "/r/n" per HTTP 
*	guidelines.
*
*/
std::string&	Response::buildAndGetResponsePhase2( void ) {

	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400 || this->status_code_ == 0) {
		createErrorBody_();
	}
	this->response_ = addHeaders_(this->response_);
	if (!this->body_.empty()) {
		this->response_ += this->body_ + CRLF;
	}
	this->response_ += CRLF;
	return (this->response_);
}

/*! \brief takes a reference to the body returned from a cgi process and returns a 
*			reference to the response string built.
*       
*
*	Builds and returns response, checking if there has been an error, adding headers as 
*	required, and appending the body passed to the function. All are separated by the 
*	CRLF or "/r/n" per HTTP guidelines.
*  
*/
std::string&	Response::buildAndGetResponsePhase2( const std::string& body ) {
	
	this->body_ = body;

	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400 || this->status_code_ == 0) {
		createErrorBody_();
	}
	this->response_ = addHeaders_(this->response_);
	if (!this->body_.empty()) {
		this->response_ += this->body_ + CRLF;
	}
	this->response_ += CRLF;
	return (this->response_);
}

/*! \brief clear method resets the response for next use
*       
*
*  More details to be filled as project progresses.
*  
*/
void	Response::clear( void ) { 	/* reset for next use */

	this->response_ = "";
	this->body_ = "";
	this->binary_data_.clear();
	this->response_mime_ = "";
	this->resource_path_ = "";
	this->resource_location_ = "";
	this->status_code_ = 0;
	this->request_ = NULL;
	this->redirect_ = false;
	this->alias_ = false;
	this->directory_listing_ = false;
}

/*! \brief	returns http status code set for the response
*
*
*	returns http status code set for the response
*
*/
int	Response::getStatusCode( void ) const {

	return (this->status_code_);
}

/*! \brief	setter for the response status code
*
*
*	Allows for client and cgi handler to set the response status code.
*
*/
void	Response::setStatusCode( unsigned int	new_code ) {

	this->status_code_ = new_code;
}

/*! \brief returns the filepath to the requested resource
*
*
*	returns the filepath to the requested resource
*
*
*/
const std::string&	Response::getResourcePath( void ) const {

	return (this->resource_path_);
}

/*! \brief returns const reference to the query string for a CGI script
*
*
*	returns const reference to the query string for a CGI script
*
*/
const std::string&	Response::getQueryString( void ) const {

	return (this->query_string_);
}

/*! \brief returns begin iterator for the stored filedata for upload
*
*
*	returns begin iterator for the stored filedata for upload
*
*/
std::vector<char>::iterator	Response::getFileDataBegin( void ) {

	return (this->file_data_.begin());
}

/*! \brief returns end iterator for the stored filedata for upload
*
*
*	returns end iterator for the stored filedata for upload
*
*/
std::vector<char>::iterator	Response::getFileDataEnd( void ) {

	return (this->file_data_.end());
}


/* CLASS PRIVATE METHODS */

/*! \brief checks for existance and file access rights for requested resource
*
*	
*	Checks for existance and access rights for requested resource.
*	Error codes may be set, 404 - not found, 403 - access not allowed
*
*/
bool	Response::validateResource_( void ) {

	if (access(this->resource_path_.c_str(), F_OK) != 0) {
		this->status_code_ = 404;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found validating resource exists: `%s'", this->resource_path_.c_str());
		return (false);
	}
	if (this->request_->getRequestLineValue("method") == "DELETE") { //set different rules here if cgi flag is set => && !this->request_->getCgiFlag()
		return (true);
	}
	else if (this->request_->getCgiFlag() && access(this->resource_path_.c_str(), X_OK) != 0) {
		this->status_code_ = 403;
		Logger::log(E_DEBUG, COLOR_CYAN, "403 execution access not allowed for cgi file: `%s'", this->request_->getRequestLineValue("uri").c_str());
		return (false);
	}
	else if (access(this->resource_path_.c_str(), R_OK) != 0) {
		this->status_code_ = 403;
		Logger::log(E_DEBUG, COLOR_CYAN, "403 read access not allowed for resource file: `%s'", this->request_->getRequestLineValue("uri").c_str());
		return (false);
	}
	return (true);
}

/****************************************** HEADER GENERATORS ******************************************/

//could make this an array of methods and call them, appending CRLF to each in a loop

/*! \brief	calls methods to add headers to the response
*
*	Appends each header to the response as needed. No errors are set here. 
*	Returns a reference to the response string. All are separated by the CRLF or "/r/n" 
*	per HTTP guidelines.
*
*/
std::string&	Response::addHeaders_( std::string& response) const {

	// std::string method = this->request_->getRequestLineValue("method");

	//always
	response += this->timeStampHeader_() + CRLF;
	response += this->contentLengthHeader_() + CRLF;
	response += this->contentLocationHeader_() + CRLF;
	//conditional headers
	if (this->body_.length() != 0)
		response += this->contentTypeHeader_() + CRLF;
	if (this->redirect_) {
		response += this->locationHeader_() + CRLF;
	}
	//between headers and body
	response += CRLF;
	return ( response );
}

/*! \brief creates the location header for a redirection, sets redirect the index page of 
*				redirect location found.
*	
*	Creates `Location' header to instruct client that resources requested are at indicated
*	location. If no index is found for the location, index is still returned as the location
*	of the resource. 
*
*	NOTES:
*	-  Index value is gaurenteed to be set by the validator.
*
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
		return "Location: " + this->resource_location_ + "/" + "index.html" + CRLF;
	}
	else {
		redirect_path = redirect_path.substr(location_pos + path_location.length());
		if (this->resource_location_ == "/")
			return "Location: " + redirect_path + CRLF;
		else
			return "Location: " + this->resource_location_ + redirect_path + CRLF;
	}
}

/*! \brief timeStampHeader private method returns header formated current time in GMT
*       
*
*  Current time in GMT is prepended with `Date: ` in the HTTP 1.1 format.
*  
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
*	returns Content-Type header based on mime type set for the response
*
*
*/
std::string	Response::contentTypeHeader_( void ) const {

	return ("Content-Type: " + this->response_mime_);
}

/*! \brief returns formated response header for Content-Length
*
*	Returns formated response header for Content-Length
*
*
*/
std::string Response::contentLengthHeader_( void ) const {
	
	return ("Content-Length: "  + intToString(this->body_.length()));
}

/*! \brief returns formated response header for Content-Location
*
*
*	Returns formated response header for Content-Location.
*	Resource_location is used always as it is overwritten in case of redirection 
*	and in case of alias the aliased location should not be sent.
*
*/
std::string Response::contentLocationHeader_( void ) const {

	return ("Content-Location: " + this->resource_location_);
	// return ("Content-Location: " + this->server_->getRoot() + this->resource_location_ + this->resource_path_);
}

/****************************************** GET ERROR PAGE ******************************************/

/*! \brief sets body of response to error content
*	
*	Creates the body of the error response either from a file given in the config or
*	generates a default error page from the ResponseCodes class.
*  
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
*	
*	If `return' key is set in the origional location from the uri 
*	- Handles a single redirection, if multiple are chained together the client
*		must send a request for each.
*  
*/
bool	Response::handleRedirection( void ) {

	if (this->server_->isKeyInLocation(this->resource_location_, "return")) {
	    this->resource_location_ = this->server_->getLocationValue(this->resource_location_, "return")->front();
		this->redirect_ = true;
	    Logger::log(E_DEBUG, COLOR_CYAN, "Redirection found for location new location : %s", this->resource_location_.c_str());
		return true;
	}
	return false;
}

/*! \brief checks for location alias and sets relvant variables
*	
*	If `alias' key is set in the origional location from the uri and sets
*	alias bool to true if so. Will also check to see if the alias points
*	to the /cgi-bin location, if so then sets flag in request.
*  
*/
void	Response::handelAlias( void ) {

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
*    setResourceLocation:
*		-  checks if the request if for cgi, rejects as invalid for not specifying a specific script
*		-  check if location is valid
*		-  calls `handlRedirection' to change location as needed
*	Error codes:
*		404 - Not Found if location does not exist
*		400 - Invalid Request if no cgi script is defined
*  
*/
void	Response::setResourceLocation( std::string& uri, bool is_dir, size_t last_slash_pos ) {

	if (is_dir) {
		this->resource_location_ = uri; //won't have root applied
		if (this->request_->getCgiFlag()) {
			this->status_code_ = 400; // invalid request if we don't allow this?
			Logger::log(E_DEBUG, COLOR_CYAN, "404 CGI script not given by request: `%s'", uri.c_str());
		}
	}
	else {
		this->resource_location_ = uri.substr(0, last_slash_pos); //path only, first part of uri
		if (this->resource_location_.empty()) {
			this->resource_location_ = "/";
		}
	}
	if (!this->server_->isLocationInServer(this->resource_location_)) {
		this->status_code_ = 404;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found while setting location and name for resource: `%s'", uri.c_str());
		return ;
	}
	if (!handleRedirection())
		handelAlias();
}



//only incase of no index will we assume index.html -> //TODO !!!this should be set when searching for the index not here. 
/*! \brief sets resource path based on verified location from the uri
*	
*    setResourcePath:
*		-  sets resource path as path to index of this page
*		-  verifies that a cgi script is listed in the server config
*       -  checks for autoindex and sets directory-listing `true` if found
*	Error codes:
*		404 - Not Found if location does not exist
*		400 - Invalid Request if no cgi script is defined
*  
*/
void	Response::setResourcePath( std::string& uri, bool is_dir, size_t last_slash_pos ) {

	if (is_dir) {
		//check for directory listing here
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
			this->status_code_ = 301; //permanently moved (chrome should use this in the future)
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
		else if (this->request_->getCgiFlag()) {
			this->resource_path_ = (this->server_->getLocationValue("/cgi-bin", "root"))->front() + "/" + filename;
		}
		else if (this->request_->getCgiFlag() && !this->server_->isScriptOnCgiList(filename)) {
			this->status_code_ = 404; //TODO: is this correct code for not allowed script?
			Logger::log(E_DEBUG, COLOR_CYAN, "404 CGI script given by request was not on approved list: `%s'", uri.c_str());
		}
	}
}

/*! \brief Extracts information from the request URI and validates 
*				the location requested
*       
*
* 	Check for the validitiy of the location and possible CGI information in 
*	the URI form the request. The existence of the files is not checked, only that 
*	they have been entered into the server at this point.
*	
*	404 - Not Found will be set if location does not exist
*  
*/
int	Response::setResourceLocationAndName( std::string uri ) {
	
	size_t	last_slash_pos = uri.find_last_of('/');

	if (last_slash_pos != std::string::npos && (last_slash_pos != uri.length() || uri == "/")) {
		std::string path;
		bool		is_dir;

		uri == "/" ? path = this->server_->getRoot() : path = this->server_->getRoot() + uri; // do cgi paths need dif check?
		is_dir = isDirectory(path) || this->server_->isLocationInServer(uri);
		setResourceLocation(uri, is_dir, last_slash_pos);
		if (this->status_code_ < 400)
			setResourcePath(uri, is_dir, last_slash_pos);
	}
	else {
		this->status_code_ = 400; //invalid request
		Logger::log(E_DEBUG, COLOR_CYAN, "URI seems to be invalid :/ : `%s'", uri.c_str());
	}
	return (this->status_code_);
}

/*
- break apart uri to determine if it is a file or directory
- find the location that corresponds to that uri
	- save the location name for looking up location info
	- save filepath to resource for access
- if cgi is requested, verify the script is on the approved list
*/


/*! \brief returns true or false if requested method is allowd for the location
*
*	Uses getLocationValue from server to get the vector of allowed methods,
*	searches them using find to see if requested method is contained in the vector.
*
*	Return : false if no methods listed or method not found and true if found.
*
*/
bool	Response::methodAllowed_( std::string method ) {

	const std::vector<std::string>*	methods;
	
	this->alias_ ? 
	methods = this->server_->getLocationValue(this->alias_location_, "allow_methods") 
	: methods = this->server_->getLocationValue(this->resource_location_, "allow_methods");
	
	if (!methods || methods->empty()) {
		return (false);
	}
	else if (std::find(methods->begin(), methods->end(), method) == methods->end()) {
		return (false);
	}
	else {
		return (true);
	}
}

/************************************************* GET *************************************************/

/*! \brief	returns the accepted formats from request in a vector
*
*	Retrieves the accepted format header fromt he request and splits on `,'
*	removing references to OWS tokens as they are not currently used.
*
*	Return: vector of strings with the mimetypes of the accepted formats
*
*/
std::vector<std::string>	Response::getAcceptedFormats( void ) {

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
	return (accepted_formats);
}

/*! \brief sets the content type MIME for the response header
*
*	Sets the content type MIME for the response header.
*	- cgi responses have `html` set as their mime type
*	- files without extensions are sent as `unknown` which is sent as octet stream
*	- otherwise if extension was not in the mime_types_ map the response is set 
*		set to error code of 415 - no supported media type
*
*/
void	Response::setMimeType( void ) {

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
		this->status_code_ = 415;
		Logger::log(E_DEBUG, COLOR_CYAN, "415 File extension not supported media type: `%s'", this->request_->getRequestLineValue("uri").c_str());
	}
}

//will uri have the resource in int -> YES ex: /blue/image.jpg; I only want to check for the blue right?
/*! \brief	implements the GET method
*
*
*
*
*/
void	Response::getMethod_( void ) {

	std::vector<std::string>	accepted_formats = getAcceptedFormats();

	setMimeType();
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
			buildBody_(this->resource_path_, std::ifstream::binary); //binary
		}
		if (this->status_code_ < 400) {
			this->status_code_ = 200; //OK, everything worked!
		}
	}
	else {
		this->status_code_ = 415;
		Logger::log(E_DEBUG, COLOR_CYAN, "415 MIME type of resource not in client `Accept` list: %s.", this->request_->getRequestLineValue("uri").c_str());
	}
}

/*! \brief saves body content to send in the body_ attribute
*
*
*
*
*/
void	Response::buildBody_( std::string& path, std::ios_base::openmode mode ) {

	std::ifstream	resource(path, mode);
	
	if (!resource.is_open() || resource.fail() || resource.bad()) {
		this->status_code_ = 404;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 cannot open resource: %s.", this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	if (mode == std::ios::binary) {
		std::cout << "Binary one here1" << std::endl;
		std::stringstream		contents;
		contents << resource.rdbuf();
		this->body_ += contents.str();
		// char	bin_char;
		// while (!resource.eof()) {
		// 	resource >> bin_char;
		// 	this->binary_data_.push_back(bin_char);
		// }
	}
	else {
		std::cout << "Text one here2" << std::endl;
		std::string line;
		while (std::getline(resource, line, '\n')) {
			if (!resource.eof())
				line += "\n";
			this->body_ += line;
		}
	}
	resource.close();
	//check if body was too large 431 or 424
}

/****************************************** HEAD ******************************************/

/*! \brief	implements HEAD method, returns 200 if resource acceptable to client
*				and 415 if not.
*
*	Checks the MIME type of the requested resource agains the acceptable formats
*	from client. If MIME is unsuported or unacceptable the error status code 
*	is set. Else to 200 OK status code is set.
*
*/
void	Response::headMethod_( void ) {

	std::vector<std::string>	accepted_formats = getAcceptedFormats();

	setMimeType();
	if (this->status_code_ >= 400) {
		return ;
	}
	if (accepted_formats.empty() || std::count(accepted_formats.begin(), accepted_formats.end(), "*/*") || std::count(accepted_formats.begin(), accepted_formats.end(), this->response_mime_)) {
		if (this->status_code_ < 400) {
			this->status_code_ = 200; //OK, everything worked!
		}
	}
	else {
		this->status_code_ = 415;
		Logger::log(E_DEBUG, COLOR_CYAN, "HEAD: 415 MIME type of resource not in client `Accept` list: %s.", this->request_->getRequestLineValue("uri").c_str());
	}
}

/****************************************** DELETE ******************************************/

/*! \brief if no cgi present, calls remove on resource path to delete file
*
*
*
*
*/
void	Response::deleteMethod_( void ) {

	if (this->request_->getCgiFlag()) {
		//maybe set the query string here?
		return ;
	}
	
	if (std::remove(this->resource_path_.c_str()) != 0 ) {
		Logger::log(E_ERROR, COLOR_RED, "DELETE METHOD, removal of resource failed : `%s'", this->request_->getRequestLineValue("uri").c_str());
		this->status_code_ = 500; //internal server error for now
	}
	else {
		this->status_code_ = 200;
	}
}

/****************************************** POST ******************************************/

void	Response::saveBodyToFile( void ) {

	const std::string& filename = this->request_->getUploadName();
	//if fname is empty then reject
	std::string file_path = this->server_->getLocationValue(this->resource_location_, "root")->front() + "/" + filename;
	std::ofstream	new_file(file_path, std::ostream::binary);//open as binary mode
	if (new_file.bad() || new_file.fail() || !new_file.is_open()) {
		this->status_code_ = 500;
		return ;
	}

	// for (std::vector<char>::const_iterator it = this->file_data_.begin(); it < this->file_data_.end(); ++it) {
	// 	new_file << *it;
	// }
	new_file << this->request_->getUploadContent();
	new_file.close();
	this->status_code_ = 201;//created
}
/*
 -process for file
 	- validate that it is multipart/form-data
	- need to get the start of the file data
	- lead up to that will have the form info with the filename
	Q's
	- if not multipart form data then what?

 -process for cgi
	- validate the form type
	- prepare the info from multipart/form-data into QUERY_STRING and file data
	- 
*/

/*! \brief	Post method currenly only works with cgi
*
*
*
*
*/
void	Response::postMethod_( void ) {

	std::string	content_type_values = this->request_->getHeaderValueByKey("Content-Type");
	bool						cgi_flag = this->request_->getCgiFlag();
	
	// if (!cgi_flag || content_type_values.empty()) {
	// 	this->status_code_ = 406; //not acceptable???
	// 	return ;
	// }
	if (content_type_values.empty()) {
		this->status_code_ = 406; //not acceptable???
		return ;
	}
	if (!cgi_flag) {
		this->saveBodyToFile();
		std::cout << "POST:  NO CGI FLAG" << std::endl;
	}
	else {
	//prepare CGI data ..
		//handle form data
		// parseMultiPartFormData(content_type_values[1]);
		std::cout << "POST:  CGI FLAG, form type not checked here" << std::endl;
		// this->query_string_ = urlEncode(this->query_string_);
	}
	// else {
	// 	//unsupported
	// 	this->status_code_ = 403; //Forbidden - couldn't find a better one to reply with unsported..., will keep looking
	// 	Logger::log(E_DEBUG, COLOR_BRIGHT_MAGENTA, "POST request without form-data detected, not currently processed, 403 Forbidden reply will send");
	// }

	//call CGI  handler here if no error code

	//just for now
	//this will be an invalid thing in the future...
	// if (this->status_code_ == 0) {
	// 	buildBody_(this->resource_path_, std::ifstream::in);
	// 	this->status_code_ = 201; //created //not actually created right now
	// }
}
