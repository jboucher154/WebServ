
#include "Response.hpp"
#include "sstream"

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
: response_(""), body_(""), binary_data_(), response_mime_(""), resource_name_(""), resource_location_(""), status_code_(0), server_(server), request_(NULL) {
	
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
		this->resource_name_ = rhs.resource_name_;
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
		Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Oh shit no sever here!");
		return ;
	}
	if (this->request_ == NULL || !this->request_->getComplete()) {
		if (this->request_ != NULL && !this->request_->getComplete() && this->request_->getChunked()) {
			this->status_code_ = 100;
			Logger::log(E_DEBUG, COLOR_BRIGHT_YELLOW, "Request is chunked and is not finished. 100 OK set!");
		}
		else {
			this->status_code_ = 400; //Bad Request
			Logger::log(E_DEBUG, COLOR_CYAN, "Response Generated before request complete.");
		}
		return ;
	}
	setResourceLocationAndName(this->request_->getRequestLineValue("uri"));
	if (!uriLocationValid_()) {
		this->status_code_ = 404;
		Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found checking uriLocationValid: `%s'", this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	if (!methodAllowed_(this->request_->getRequestLineValue("method"))) {
		this->status_code_ = 405;
		Logger::log(E_DEBUG, COLOR_CYAN, "405 Method not allowed, %s, on server %s for uri: `%s'", this->request_->getRequestLineValue("method").c_str(), this->server_->getServerName().c_str() , this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	if (this->status_code_ < 400) {
		for (unsigned long i = 0; i < (sizeof(possible_methods)/sizeof(std::string)); i++) {
			// std::cout << "DUBUGGING WITHOUT LOGGER -> METHOD: " << this->request_->getRequestLineValue("method") << " POSSIBLE METHOD: " << possible_methods[i]<< std::endl; 
			if (this->request_->getRequestLineValue("method") == (possible_methods[i])) {
				(this->*methods[i])();
				break ;
			}
		}
	}
}

//should it return a string?
/*! \brief get method returns the response as a c string.
*       
*
*  Currently returns response from ResponseCodes class based on current status_code_.
*  More details to be filled as project progresses.
*  
*/
std::string&	Response::get( /*socket to write to?*/ ) {

	// std::string response;
	
	this->response_ = ResponseCodes::getCodeStatusLine(this->status_code_);
	if (this->status_code_ >= 400) {
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
	this->resource_name_ = "";
	this->resource_location_ = "";
	this->status_code_ = 0;
	// this->server_ = NULL;
	this->request_ = NULL;
}

/* CLASS PRIVATE METHODS */

// https://www.iana.org/assignments/media-types/media-types.xhtml
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

	//message

	//model

	//multipart

	//video
	// Response::mime_types_["jpeg"] = {"jpeg", "video/jpeg", true};
	Response::mime_types_["mp4"] = "video/mp4";
	Response::mime_types_["raw"] = "video/raw";
	Response::mime_types_["MPV"] = "video/MPV";

}

/****************************************** HEADER GENERATORS ******************************************/

//could make this an array of methods and call them, appending CRLF to each in a loop
std::string&	Response::addHeaders_( std::string& response) const {

	std::string method = this->request_->getRequestLineValue("method");

	//always
	response += this->timeStampHeader_() + CRLF;
	response += this->contentLengthHeader_() + CRLF;
	response += this->contentTypeHeader_() + CRLF;
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

std::string	Response::contentTypeHeader_( void ) const {

	return ("Content-Type: " + this->response_mime_);
}

std::string Response::contentLengthHeader_( void ) const {
	
	return ("Content-Length: "  + intToString(this->body_.length()));
}

std::string Response::contentLocationHeader_( void ) const {

	return ("Content-Location: " + this->server_->getRoot() + this->resource_location_ + this->resource_name_);
}

/****************************************** SHARED CHECKS BEFORE METHOD ******************************************/

//check for valid location based on processsed uri
bool	Response::uriLocationValid_( void )  {

	if (this->resource_location_.empty()) {
		setResourceLocationAndName(this->request_->getRequestLineValue("uri"));
	}
	if (this->server_->isLocationInServer(this->resource_location_)) {
		return (true);
	}
	else {
		return (false);
	}
}

// static void parseNameFromContentDisposition(std::string& content_disposition) {

// }

//sets resource location and name
//do i want to separate these?
//can set error code to not found
void	Response::setResourceLocationAndName( std::string uri ) {

	size_t	last_slash_pos = uri.find_last_of('/');

	if (last_slash_pos != std::string::npos && (last_slash_pos != uri.length() || uri == "/")) { // is there and not at end of string
		//build path with uri check if dir
		std::string path;
		if (uri == "/")
			path = this->server_->getRoot();
		else
			path = this->server_->getRoot() + uri;
		if (isDirectory(path)) {
			this->resource_location_ = uri;
			//does the location exist in the server?
			if (this->request_->getRequestLineValue("method") == "GET") {
				if (this->server_->isKeyInLocation(this->resource_location_, "index")) {
					this->resource_name_ = this->server_->getLocationValue(this->resource_location_, "index")->front();
				}
				else if (this->resource_location_ == "/") {
					this->resource_name_ = this->server_->getIndex();
				}
				else {
					this->status_code_ = 404;
					Logger::log(E_DEBUG, COLOR_CYAN, "404 Location not found while setting location and name for resource: `%s'", uri.c_str());
				}
			}
			else if (this->request_->getRequestLineValue("method") == "POST") {
				this->resource_name_ = this->request_->getHeaderValueByKey("Content-Disposition");
			}
		}
		else {
			this->resource_location_ = uri.substr(0, last_slash_pos + 1);
			this->resource_name_ = uri.substr(last_slash_pos + 1);
		}
	}
}

//defalut to deny method if none entered
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

/*************************************************GET*************************************************/

//ignore OWS tokens for now
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

//should check for redirection/ alias too
std::string	Response::buildResourcePath( void ) {

	std::string resource_path;

	if (this->server_->isKeyInLocation(this->resource_location_, "root")) {
		resource_path = (this->server_->getLocationValue(this->resource_location_, "root"))->front();
	}
	else {
		resource_path = this->server_->getRoot();
	}
	// resource_path += this->request_->getRequestLineValue("uri");
	resource_path += this->resource_location_ + this->resource_name_;
	return (resource_path);
}

void	Response::setMimeType( void ) {

	size_t	extension_start = this->resource_name_.find_last_of('.');

	if (extension_start == std::string::npos) {
		this->response_mime_ = Response::mime_types_["unknown"];
	}
	else {
		std::string extension = this->resource_name_.substr(extension_start + 1);
		this->response_mime_ = Response::mime_types_[extension];
	}
	if (this->response_mime_.empty()) {
		this->status_code_ = 415;
		Logger::log(E_DEBUG, COLOR_CYAN, "415 File extension not supported media type: `%s'", this->request_->getRequestLineValue("uri").c_str());
	}
}

//what to do if no accepted format specified? can take anything...
//will uri have the resource in int -> YES ex: /blue/image.jpg; I only want to check for the blue right?
void	Response::getMethod_( void ) {

	std::vector<std::string>	accepted_formats = getAcceptedFormats();
	std::string 				resource_path = buildResourcePath();

	setMimeType();
	if (this->status_code_ >= 400) {
		return ;
	}
	if (accepted_formats.empty() || std::count(accepted_formats.begin(), accepted_formats.end(), "*/*") || std::count(accepted_formats.begin(), accepted_formats.end(), this->response_mime_)) {
		if (this->response_mime_.compare(0, 4, "text") == 0) {
			buildBody_(resource_path, std::ifstream::in);
		}
		else {
			buildBody_(resource_path, std::ifstream::binary); //binary
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

//only setup for text, will need to eval type and either copy text or binary info
void	Response::buildBody_( std::string& path, std::ios_base::openmode mode ) {

	//should set something else for resource_location incase it is diff than uri (specified file like index.html, not just `/`)
	std::ifstream	resource(path, mode); // check it it exists or is a dir...
	
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

void	Response::headMethod_( void ) {

	this->status_code_ = 501;
}

void	Response::deleteMethod_( void ) {

	this->status_code_ = 501;
}

/****************************************** POST ******************************************/

std::string	Response::getExtension_( void ) { //set extention for file upload

	std::string	content_type = this->request_->getHeaderValueByKey("Content-Type");

	if (content_type.empty()) {
		this->status_code_ = 400;
	}
	for (std::map<std::string, std::string>::const_iterator it = this->mime_types_.begin(); it != this->mime_types_.end(); it++) {
		if (it->second == content_type) {
			return (it->first);
		}
	}
	return ("unknown"); //return empty???
}

// should I check if it already exists? if it does should it be overwritten?
// do I need to make my own file name?
std::string	Response::createFile_( std::string& extension ) {
	
	std::string path;

	if (extension == "unknown") {
		extension = "";
	}
	path = buildResourcePath();
	std::ofstream	newFile(path + extension);
	if (newFile.fail() || newFile.bad()) {
		this->status_code_ = 500;
		return "";
	}
	else {
		newFile.close();
		return path + extension;
	}
}

void	Response::uploadFile_( std::string filepath ) {

	std::ofstream	upload(filepath, std::iostream::binary);// need open mode, lets do binary for now...

	if (upload.fail() || upload.bad()) {
		this->status_code_ = 500;
		return ;
	}
	for (std::vector<char>::iterator it = this->request_->getBinaryBodyBegin(); it != this->request_->getBinaryBodyEnd(); it++) {
		upload << *it;
	}
	upload.close();
}

// std::string	Response::getBoundry_( void ) {

// }

// std::string Response::MimeTypeFromContentType_( void ) {
	
// 	std::string content_type = this->request_->getHeaderValueByKey("Content-Type");
// 	//ss
// 	//mime
// 	//boundary
// }	

std::vector<std::string> 	Response::GetContentTypeValues_( void ) {
	
	std::string 				content_type = this->request_->getHeaderValueByKey("Content-Type");
	std::stringstream			ss(content_type);
	std::string					value;
	std::vector<std::string>	values;

	while (getline(ss, value, ';' )) {
		if (value.front() == ' ') {
			value.erase(0, 1);//removes ws
		}
		values.push_back(value);
	}
}

//check that host is the refferer? 
void	Response::postMethod_( void ) {

	std::vector<std::string>	content_type_values = this->GetContentTypeValues_();
	
	if (content_type_values.front() == "multipart/form-data") {
		//handle form data
		t_FormData	form_data;

	}
	else if (content_type_values.front() == "application/x-www-form-urlencoded") {
		// send url encoded string to cgi
	}
	else {
		//unsoported type
	}
	//parse the form data

	// if (!this->request_->getHeaderValueByKey("Content-Disposition").empty()) {
	// 	std::cout << "Content disoposition exists!!" << std::endl;
	// } else {
	// 	std::cout << "No Content disoposition. sad." << std::endl;
	// }
	// std::string extension = getExtension_();
	// std::string	resource_location; // need to set this for response OR set the uri in request to location?
	// // make file 
	// resource_location = this->createFile_(extension);
	// std::cout << "resource Location for created file: " << resource_location << "resource name: " << this->resource_name_ << std::endl;
	// // put stuff into it
	// this->uploadFile_(resource_location);
	// //copy same into body to include in response
	// this->buildBody_(resource_location, std::iostream::binary);
	// if (this->status_code_ == 0) {
	// 	this->status_code_ = 201; //created
	// }
}
