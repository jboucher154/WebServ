
#include "Response.hpp"

/* CONSTRUCTORS */

/*! \brief Default constructor is private.
*       
*
*  Default constructor is private.
*  
*/
Response::Response( void ) { /* default constructor */}

/*! \brief Default constructor intializes empty response and body. Default status code set to 501 for MVP.
*       
*
*  Default constructor intializes empty response and body. Default status code set to 501 for MVP.
*  More details to be filled as project progresses.
*/
Response::Response( Server* server )
: response_(""), body_(""), response_mime_(""), status_code_(501), server_(server), request_(NULL) {
	
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

/*! \brief Assignment operator is private and cannot be used.
*       
*
*   Assignment operator is private and cannot be used.
*  
*/
Response&	Response::operator=( const Response& rhs ) {

	if (this != &rhs) {

		this->response_ = rhs.response_;
		this->status_code_ = rhs.status_code_;
		this->body_ = rhs.body_;
		this->server_ = rhs.server_;
		this->request_ = rhs.request_;
		this->response_mime_ = rhs.response_mime_;
	}
	return (*this);
}

/* CLASS PUBLIC METHODS */


/*! \brief generate method creates the response given a Request object.
*       
*
*  Not yet implemented.
*  
*/
void	Response::generate( Request* request ) {

	std::string			possible_methods[] ={ "GET", "HEAD", "POST", "DELETE"};
	response_methods_	methods = { &Response::getMethod_, &Response::headMethod_, &Response::postMethod_, &Response::deleteMethod_ };

	this->request_ = request;
	if (this->request_ == NULL || !this->request_->getComplete()) {
		this->status_code_ = 400; //Bad Request
		Logger::log(E_DEBUG, COLOR_YELLOW, "Response Generated before request complete.");
		return ;
	}
	if (!uriLocationValid_(this->request_->getRequestLineValue("uri"))) {
		this->status_code_ = 404; //Not Found
		Logger::log(E_DEBUG, COLOR_YELLOW, "404 Location not found: %s.", this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	if (!methodAllowed_(this->request_->getRequestLineValue("method"))) { //could hardcode GET
		this->status_code_ = 405; // Method Not Allowed
		Logger::log(E_DEBUG, COLOR_YELLOW, "405 Method not allowed, GET: %s.", this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	for (unsigned long i = 0; i < (sizeof(possible_methods)/sizeof(std::string)); i++) {
		if (this->request_->getRequestLineValue("method").compare(possible_methods[i])) {
			(this->*methods[i])();
			break ;
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
const char*	Response::get( /*socket to write to?*/ ) {

	std::string response;
	
	response = ResponseCodes::getCodeStatusLine(this->status_code_);
	this->body_ = ResponseCodes::getCodeElementBody(this->status_code_);
	response += this->timeStampHeader_() + CRLF;
	response += "Content-Length : ";
	response += int_to_string(this->body_.length());
	response += "\r\n\r\n";
	response += this->body_ + CRLF + CRLF;
	return (response.c_str());

	//add response line based on code
	//add additional headers (at least Date, content-length, content-type)
	//add break line
	//add body
	//add end marker
	//send
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
	this->response_mime_ = "";
	this->status_code_ = 0;
	this->server_ = NULL;
	this->request_ = NULL;
}

/* CLASS PRIVATE METHODS */

// https://www.iana.org/assignments/media-types/media-types.xhtml
void	Response::intializeMimeTypes( void ) {

	//text
	Response::mime_types_["html"] = {"html", "text/html", false};
	Response::mime_types_["css"] = {"css", "text/css", false};
	Response::mime_types_["txt"] = {"txt", "text/plain", false};
	Response::mime_types_["js"] = {"js", "text/javascript", false};
	Response::mime_types_["md"] = {"md", "text/markdown", false};
	Response::mime_types_["csv"] = {"csv", "text/csv", false};
	Response::mime_types_["rtf"] = {"rtf", "text/rtf", false};
	Response::mime_types_["xml"] = {"xml", "text/xml", false};

	//application
	Response::mime_types_["unknown"] = {"unknown", "application/octet-stream", true};//unkown binary file
	Response::mime_types_["pdf"] = {"pdf", "application/pdf", true};
	Response::mime_types_["msword"] = {"msword", "application/msword", true};
	Response::mime_types_["zip"] = {"zip", "application/zip", true};

	//audio
	// Response::mime_types_["mp4"] = {"mp4", "audio/mp4", true};

	//font
	Response::mime_types_["ttf"] = {"ttf", "font/ttf", true};

	//image
	Response::mime_types_["bmp"] = {"bmp", "image/bmp", true};
	Response::mime_types_["gif"] = {"gif", "image/gif", true};
	Response::mime_types_["jpeg"] = {"jpeg", "image/jpeg", true};
	Response::mime_types_["png"] = {"png", "image/png", true};
	Response::mime_types_["tiff"] = {"tiff", "image/tiff", true};

	//message

	//model

	//multipart

	//video
	// Response::mime_types_["jpeg"] = {"jpeg", "video/jpeg", true};
	Response::mime_types_["mp4"] = {"mp4", "video/mp4", true};
	Response::mime_types_["raw"] = {"raw", "video/raw", true};
	Response::mime_types_["MPV"] = {"MPV", "video/MPV", true};

}

/****************************************** HEADER GENERATORS ******************************************/

//check if logging time stamp can do the same thing
/*! \brief timeStampHeader private method returns header formated current time in GMT
*       
*
*  Current time in GMT is prepended with `Date: ` in the HTTP 1.1 format.
*  
*/
std::string		Response::timeStampHeader_( void ) const{

	time_t	now = time(0);
	struct tm tstruct;
	std::string	time_stamp("Date: ");
	char	buffer[100];

	tstruct = *gmtime(&now);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %X GMT", &tstruct);
	time_stamp += buffer;
	return time_stamp;
}

/****************************************** SHARED CHECKS BEFORE METHOD ******************************************/

bool	Response::uriLocationValid_( std::string uri )  {

	if (this->server_->getLocationKeys(this->request_->getRequestLineValue("uri")).empty()) {
		return (false);
	}
	else {
		return (true);
	}
}

bool	Response::methodAllowed_( std::string method ) {

	//should get a reference or pointer to the vector
	const std::vector<std::string>*	methods = this->server_->getLocationValue(this->request_->getRequestLineValue("uri"), method);
	
	if (methods->empty()) {
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

//will uri have the resource in int -> YES ex: /blue/image.jpg; I only want to check for the blue right?
void	Response::getMethod_( void ) {

	std::string accepted_formats = this->request_->getHeaderValueByKey("Accept");
	std::string MIME_type; //file type for header... //set mime type
	std::string resource_path; //file type for header...

	//build resource path
	if (this->server_->isKeyInLocation(this->request_->getRequestLineValue("uri"), "root")) {
		resource_path = (this->server_->getLocationValue(this->request_->getRequestLineValue("uri"), "root"))->front();
	}
	else {
		resource_path = this->server_->getRoot();
	}
	resource_path += this->request_->getRequestLineValue("uri");
	
	buildPlainTextBody_(resource_path, std::ifstream::in);
	//for binary
	buildPlainTextBody_(resource_path, std::ifstream::binary);
}

//should check for redirection/ alias too

//only setup for text, will need to eval type and either copy text or binary info
//need to set MIME type
void	Response::buildPlainTextBody_( std::string, std::ios_base::openmode mode ) {

	//should set something else for resource_location incase it is diff than uri (specified file like index.html, not just `/`)
	std::ifstream	resource(this->request_->getRequestLineValue("uri"), mode); // check it it exists or is a dir...
	std::string		line;
	
	if (!resource.is_open() || resource.fail() || resource.bad()) {
		this->status_code_ = 404;
		Logger::log(E_DEBUG, COLOR_YELLOW, "404 cannot open resource: %s.", this->request_->getRequestLineValue("uri").c_str());
		return ;
	}
	while (std::getline(resource, line)) {
		if (!resource.eof())
			line += "\n";
		this->body_ += line;
	}
	resource.close();
}

// void	Response::buildBinaryTextBody_( void ) {


// }

void	Response::headMethod_( void ) {

}

void	Response::deleteMethod_( void ) {

}

void	Response::postMethod_( void ) {

}
