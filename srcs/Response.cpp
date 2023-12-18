
#include "Response.hpp"
#include <sstream>

std::map<std::string, std::string>	Response::mime_types_;

/* CONSTRUCTORS */

/*! \brief Default constructor is private.
*       
*
*  Default constructor is private.
*  
*/
Response::Response( void ) { /* default constructor */}

/*! \brief Constructor intializes empty response and body; sets status code to 0.
*       
*
*  Default constructor intializes empty response and body. Default status code set to 501 for MVP.
*  More details to be filled as project progresses.
*/
Response::Response( Server* server )
: response_(""), body_(""), binary_data_(), response_mime_(""), resource_path_(""), resource_location_(""), status_code_(0), server_(server), request_(NULL) {
	
	if (Response::mime_types_.empty()) {
		intializeMimeTypes();
	}
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

	if (Response::mime_types_.empty()) {
		intializeMimeTypes();
	}
	if (this != &rhs) {

		this->response_ = rhs.response_;
		this->status_code_ = rhs.status_code_;
		this->body_ = rhs.body_;
		this->binary_data_ = rhs.binary_data_;
		this->server_ = rhs.server_;
		this->request_ = rhs.request_;
		this->response_mime_ = rhs.response_mime_;
		this->resource_location_ = rhs.resource_location_;
		this->resource_path_ = rhs.resource_path_;
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */


/*! \brief generate method check if request is possible and calls the function for 
*				the method requestsed given a Request object.
*       
*
*  
*  
*/
void	Response::generate( Request* request ) {

	std::string			possible_methods[] ={ "GET", "HEAD", "POST", "DELETE"};
	response_methods_	methods = { &Response::getMethod_, &Response::headMethod_, &Response::postMethod_, &Response::deleteMethod_ };

	this->request_ = request;
	if (this->server_ == NULL) {
		Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Response server is NULL");
		return ;
	}
	if (this->request_ == NULL || !this->request_->getComplete()) {
		if (this->request_ != NULL && !this->request_->getComplete() && this->request_->getChunked()) {
			this->status_code_ = 100;
			Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Request is chunked and is not finished. 100 OK set!");
		}
		else {
			this->status_code_ = 400; //Bad Request
			Logger::log(E_DEBUG, COLOR_CYAN, "Bad request, Response Generated before request complete.");
		}
		return ;
	}
	if (setResourceLocationAndName(this->request_->getRequestLineValue("uri")) >= 400) {
		return ;
	}
	if (!methodAllowed_(this->request_->getRequestLineValue("method"))) {
		this->status_code_ = 405;
		Logger::log(E_DEBUG, COLOR_CYAN, "405 Method not allowed, %s, on server %s for uri: `%s'", this->request_->getRequestLineValue("method").c_str(), this->server_->getServerName().c_str() , this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	//validate resource existance and access
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


/*! \brief get method returns the response as a c string.
*       
*
*  Currently returns response from ResponseCodes class based on current status_code_.
*  More details to be filled as project progresses.
*  
*/
std::string&	Response::get( void ) {

	// std::string response;
	
	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400 || this->status_code_ == 0) { // any code that should trigger the mnimal response
		//check it there is fancy error page in server (remember to check access rights to it)
		this->body_ = ResponseCodes::getCodeElementBody(this->status_code_);
		this->response_mime_ = Response::mime_types_["html"];
	}
	this->response_ = addHeaders_(this->response_);
	if (!this->body_.empty()) {
		this->response_ += this->body_ + CRLF;
	}
	this->response_ += CRLF;
	// this->response_ = response;
	return (this->response_);
}

/*! \brief get method returns the response as a c string.
*       
*
*  Currently returns response from ResponseCodes class based on current status_code_.
*  More details to be filled as project progresses.
*  
*/
std::string&	Response::get( const std::string& body ) {
	
	this->body_ = body;

	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400 || this->status_code_ == 0) { // any code that should trigger the minmal response
		//check it there is fancy error page in server
		this->body_ = ResponseCodes::getCodeElementBody(this->status_code_);
		this->response_mime_ = Response::mime_types_["html"];
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
}

/*! \brief	returns http status code set for the response
*
*
*
*
*/
int	Response::getStatusCode( void ) const {

	return (this->status_code_);
}

/*! \brief	setter for the response status code
*
*
*
*
*/
void	Response::setStatusCode( unsigned int	new_code ) {

	this->status_code_ = new_code;
}

/*! \brief returns the filepath to the requested resource
*
*
*
*
*/
const std::string&	Response::getResourcePath( void ) const {

	return (this->resource_path_);
}

/*! \brief returns const reference to the query string for a CGI script
*
*
*
*
*/
const std::string&	Response::getQueryString( void ) const {

	return (this->query_string_);
}

/*! \brief returns begin iterator for the stored filedata for upload
*
*
*
*
*/
std::vector<std::string>::iterator	Response::getFileDataBegin( void ) {

	return (this->file_data_.begin());
}

/*! \brief returns end iterator for the stored filedata for upload
*
*
*
*
*/
std::vector<std::string>::iterator	Response::getFileDataEnd( void ) {

	return (this->file_data_.end());
}


/* CLASS PRIVATE METHODS */

/*! \brief
*
*
*	
*
* 	Reference : https://www.iana.org/assignments/media-types/media-types.xhtml
*/
void	Response::intializeMimeTypes( void ) {

	//text
	Response::mime_types_["html"] = "text/html";
	Response::mime_types_["css"] = "text/css";
	Response::mime_types_["txt"] = "text/plain";
	Response::mime_types_["js"] = "text/javascript";
	Response::mime_types_["md"] = "text/markdown";
	Response::mime_types_["csv"] = "text/csv";
	Response::mime_types_["rtf"] = "text/rtf";
	Response::mime_types_["xml"] = "text/xml";

	//application
	Response::mime_types_["unknown"] = "application/octet-stream";//unkown binary file
	Response::mime_types_["pdf"] = "application/pdf";
	Response::mime_types_["msword"] = "application/msword";//?
	Response::mime_types_["zip"] = "application/zip";

	//audio
	// Response::mime_types_["mp4"] = {"mp4", "audio/mp4";

	//font
	Response::mime_types_["ttf"] = "font/ttf";

	//image
	Response::mime_types_["bmp"] = "image/bmp";
	Response::mime_types_["gif"] = "image/gif";
	Response::mime_types_["jpg"] = "image/jpeg";
	Response::mime_types_["png"] = "image/png";
	Response::mime_types_["tiff"] = "image/tiff";
	Response::mime_types_["ico"] = "image/x-icon";

	//message


	//model
	//multipart

	//video
	// Response::mime_types_["jpeg"] = {"jpeg", "video/jpeg", true};
	Response::mime_types_["mp4"] = "video/mp4";
	Response::mime_types_["raw"] = "video/raw";
	Response::mime_types_["MPV"] = "video/MPV";

}

/*! \brief
*
*
*
*
*/
bool	Response::validateResource_( void ) {

	if (access(this->resource_path_.c_str(), F_OK) != 0) {
		this->status_code_ = 404;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found validating resource exists: `%s'", this->request_->getRequestLineValue("uri").c_str());
		return (false);
	}
	if (this->request_->getRequestLineValue("method") == "DELETE") {
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
/*! \brief
*
*
*
*
*/
std::string&	Response::addHeaders_( std::string& response) const {

	std::string method = this->request_->getRequestLineValue("method");

	//always
	response += this->timeStampHeader_() + CRLF;
	response += this->contentLengthHeader_() + CRLF;
	response += this->contentTypeHeader_() + CRLF; //cgi test/html
	response += this->contentLocationHeader_() + CRLF;
	//between headers and body
	response += CRLF;
	return ( response );
}

//check if logging time stamp can do the same thing
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
*
*
*
*/
std::string	Response::contentTypeHeader_( void ) const {

	return ("Content-Type: " + this->response_mime_);
}

/*! \brief returns formated response header for Content-Length
*
*
*
*
*/
std::string Response::contentLengthHeader_( void ) const {
	
	return ("Content-Length: "  + intToString(this->body_.length()));
}

/*! \brief returns formated response header for Content-Location
*
*
*
*
*/
std::string Response::contentLocationHeader_( void ) const {

	return ("Content-Location: " + this->resource_location_);
	// return ("Content-Location: " + this->server_->getRoot() + this->resource_location_ + this->resource_path_);
}

/****************************************** SHARED CHECKS BEFORE METHOD ******************************************/

/*! \brief Extracts information from the request URI and validates 
*				the location requested
*       
*
* 	Check for the validitiy of the location and possible CGI information in 
*	the URI form the request. The existence of the files is not checked, only that 
*	they have been entered into the server at this point.
*	
*	404 - Not Found is the only error code set in this function
*  
*/
int	Response::setResourceLocationAndName( std::string uri ) {
	
	/* NEED TO ADD HANDLING FOR ALIAS AND REDIRECTION HERE */
	size_t	last_slash_pos = uri.find_last_of('/');

	if (last_slash_pos != std::string::npos && (last_slash_pos != uri.length() || uri == "/")) {
		std::string path;
		if (uri == "/") // in future make different check for cgi paths
			path = this->server_->getRoot();
		else
			path = this->server_->getRoot() + uri;
		if (isDirectory(path)) {
			this->resource_location_ = uri;
			if (this->request_->getCgiFlag()) {
				this->status_code_ = 404;
				Logger::log(E_DEBUG, COLOR_CYAN, "404 CGI script not given by request: `%s'", uri.c_str());
			}
			else if (!this->server_->isLocationInServer(this->resource_location_)) {
				this->status_code_ = 404;
				Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found while setting location and name for resource: `%s'", uri.c_str());
			}
			else if (this->server_->isKeyInLocation(this->resource_location_, "index")) {
				this->resource_path_ = this->server_->getLocationValue(this->resource_location_, "index")->front(); //could be empty
			}
			else if (this->resource_location_ == "/") {
				this->resource_path_ = this->server_->getIndex();
			}
			else {
				this->status_code_ = 404;
				Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found while setting location and name for resource: `%s'", uri.c_str());
			}
		}
		else {
			this->resource_location_ = uri.substr(0, last_slash_pos + 1); //path only
			std::string	filename = uri.substr(last_slash_pos + 1);
			if (!this->request_->getCgiFlag())
				this->resource_path_ =  this->server_->getRoot() + uri;
			else if (this->request_->getCgiFlag()) {
				this->resource_path_ =  "." + uri; //need to check this for correct path
			}
			else if (this->request_->getCgiFlag() && !this->server_->isScriptOnCgiList(filename)) {
				this->status_code_ = 404;
				Logger::log(E_DEBUG, COLOR_CYAN, "404 CGI script given by request was not on approved list: `%s'", uri.c_str());
			}
		}
	}
	return (this->status_code_);
}

/*! \brief returns true or false if requested method is allowd for the location
*
*	Uses getLocationValue from server to get the vector of allowed methods,
*	searches them using find to see if requested method is contained in the vector.
*
*	Return : false if no methods listed or method not found and true if found.
*
*/
bool	Response::methodAllowed_( std::string method ) {

	const std::vector<std::string>*	methods = this->server_->getLocationValue(this->resource_location_, "allow_methods");
	
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
*
*
*
*/
void	Response::setMimeType( void ) {

	size_t	extension_start = this->resource_path_.find_last_of('.');

	if (this->request_->getCgiFlag()) {
		this->response_mime_ = Response::mime_types_["html"];
		return ;
	}
	if (extension_start == std::string::npos) {
		this->response_mime_ = Response::mime_types_["unknown"];
	}
	else {
		std::string extension = this->resource_path_.substr(extension_start + 1);
		this->response_mime_ = Response::mime_types_[extension];
	}
	if (this->response_mime_.empty()) {
		this->status_code_ = 415;
		Logger::log(E_DEBUG, COLOR_CYAN, "415 File extension not supported media type: `%s'", this->request_->getRequestLineValue("uri").c_str());
	}
}

//what to do if no accepted format specified? can take anything...
//will uri have the resource in int -> YES ex: /blue/image.jpg; I only want to check for the blue right?
/*! \brief
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

/*! \brief
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

/*! \brief	parses multiform data into a the query string and stores any file data int file_data_ vector
*
*
*
*
*/
void	Response::parseMultiPartFormData( std::string& boundary ) {

	if (boundary.empty()) {
		this->query_string_ = "invalid boundary found";
		this->status_code_ = 400; //invalid request
		return ;
	}
	boundary = boundary.erase(0, 9);
	boundary = "--" + boundary;
	std::string	file_datum;
	bool		file_present = false;

	for (std::vector<std::string>::iterator it = this->request_->getBodyBegin(); it != this->request_->getBodyEnd(); it++) {
		while (it != this->request_->getBodyEnd() && *it != boundary ) {
			std::stringstream	ss(*it);
			std::string	segment;

			ss >> segment;
			if (strncmp(it->c_str(), "Content-Disposition", 19) == 0) {
				while (!ss.eof()) {
					ss >> segment;
					if (segment == "form-data;")
						query_string_ += "type=form_data ";
					else {
						if (strncmp(segment.c_str(), "filename=", 9) == 0)
							file_present = true;
						query_string_ += segment + " ";
					}
				}
			}
			else if (strncmp(it->c_str(), "Content-Type", 12) == 0) {
				ss >> segment;
				query_string_ += "content-type=" + segment + " ";
			}
			else {
				if (file_present)
					file_datum += *it;
				else if (!segment.empty())
					query_string_ += "value=" + *it + " ";
			}
			it++;
		}
		if (!file_datum.empty()) {
			this->file_data_.push_back(file_datum);
			file_datum.clear();
			file_present = false;
		}
		if (it == this->request_->getBodyEnd())
			break ;
	}
}


/*! \brief returns vector of values from the Content-Type request Header
*
*
*
*
*/
std::vector<std::string> 	Response::GetContentTypeValues_( void ) {
	
	std::string 				content_type = this->request_->getHeaderValueByKey("Content-Type");
	std::stringstream			ss(content_type);
	std::string					value;
	std::vector<std::string>	values;

	while (getline(ss, value, ';' )) {
		if (value.front() == ' ') {
			value.erase(0, 1);
		}
		while (*(value.end()) == '\r' || *(value.end()) == '\n') {
			value.pop_back();
		}
		values.push_back(value);
	}
	return (values);
}

/*! \brief
*
*
*
*
*/
void	Response::postMethod_( void ) {

	std::vector<std::string>	content_type_values = this->GetContentTypeValues_();
	bool						cgi_flag = this->request_->getCgiFlag();
	
	if (!cgi_flag || content_type_values.empty()) {
		this->status_code_ = 406; //not acceptable???
		return ;
	}
	//prepare CGI data ..
	if (content_type_values.front() == "multipart/form-data") {
		//handle form data
		parseMultiPartFormData(content_type_values[1]);
		this->query_string_ = urlEncode(this->query_string_);
	}
	else if (content_type_values.front() == "application/x-www-form-urlencoded") { //assume no data for now
		// send url encoded string to cgi
		if ((*(this->request_->getBodyBegin())).find("filename=") != std::string::npos) {
			// check it includes data for a file. can set flag for the cgi if needed
			Logger::log(E_DEBUG, COLOR_BRIGHT_MAGENTA, "File included in URL encoded string of POST request");
		}
		query_string_ = *(this->request_->getBodyBegin());
	}
	else {
		//unsupported
		this->status_code_ = 403; //Forbidden - couldn't find a better one to reply with unsported..., will keep looking
		Logger::log(E_DEBUG, COLOR_BRIGHT_MAGENTA, "POST request without form-data detected, not currently processed, 403 Forbidden reply will send");
	}

	//call CGI  handler here if no error code

	//just for now
	//this will be an invalid thing in the future...
	// if (this->status_code_ == 0) {
	// 	buildBody_(this->resource_path_, std::ifstream::in);
	// 	this->status_code_ = 201; //created //not actually created right now
	// }
}
