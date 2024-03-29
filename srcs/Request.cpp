
#include "Request.hpp"

/* CONSTRUCTORS */

/*! \brief default constructor, zeros all attributes on construction
*
*	Default constructor, zeros all attributes on construction.
*    
*/
Request::Request( void ) 
: body_size_(0), 
body_len_received_(0), 
chunked_(false), 
keep_alive_(false), 
cgi_flag_(false), 
headers_complete(false), 
complete_(false), 
host_name_(""),
port_(0), 
raw_body_(""), 
processed_body_(""),
file_content_(""),
file_name_(""),
body_vector_(),
file_upload_(false),
file_mime_(""),
status_code_(E_UNSET),
query_encode_(false) {

	/* default constructor */
}

/*! \brief copy constructor calls copy assignment operator
*
*	
*  Copy constructor calls copy assignment operator.
*/
Request::Request( const Request& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief default destructor
*
*	default destructor
*/
Request::~Request( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

/*! \brief copy assignment operator copies all values from rhs
*
* 	Copy assignment operator copies all values from rhs.
*	
* 	@param rhs Request object whose values are to be copied
*	@return @b Request& request on the left-hand side
*/
Request&	Request::operator=( const Request& rhs ) {

	if (this != &rhs ) {
		this->body_size_ = rhs.body_size_;
		this->body_len_received_ = rhs.body_len_received_;
		this->chunked_ = rhs.chunked_;
		this->keep_alive_ = rhs.keep_alive_;
		this->request_line_ = rhs.request_line_;
		this->headers_ = rhs.headers_;
		this->raw_body_ = rhs.raw_body_;
		this->processed_body_ = rhs.processed_body_;
		this->body_vector_ = rhs.body_vector_;
		this->complete_ = rhs.complete_;
		this->host_name_ = rhs.host_name_;
		this->port_ = rhs.port_;
		this->cgi_flag_ = rhs.cgi_flag_;
		this->file_upload_ = rhs.file_upload_;
		this->file_mime_ = rhs.file_mime_;
		this->status_code_ = rhs.status_code_;
		this->file_content_ = rhs.file_content_;
		this->file_name_ = rhs.file_name_;
		this->query_encode_ = rhs.query_encode_;
	}
	return *this;
}

/************** CLASS PUBLIC METHODS **************/

/*! \brief add to any part of the Request, directs to correct private parsing function
*       
*	Character buffer is converted to string and stringstream to process into the relevant
*	request part: request line, header, or body. Request attributes are set and checked
*	based on headers, then body is saved and parsed using the total bytes read from the 
*	recv() call to guide the parsing. If exceptions for string conversions occur, they are 
*	caught and the error printed.
*	After the headers are complete the timer for the request is started.
*
*	@param to_add c-style string to be added to the request
*	@param bytes_read size_t of the amount of bytes read with recv()
*/
void	Request::add( char* to_add, size_t bytes_read ) {
	
	std::stringstream	ss(to_add);
	std::string			line;
	std::string			string_add(to_add, bytes_read);

	try {
		while (!ss.eof() && headers_complete == false) {
			if (!getline(ss, line, '\n'))
				break ;
			if (this->request_line_.empty()) {
				this->parseRequestLine_(line);
			}
			else if (line.compare("\r") == 0) {
				this->headers_complete = true;
				this->setRequestAttributes_();
				time(&this->request_start_time_);
			}
			else {
				this->parseHeader_(line);
			}
			if (this->status_code_ > 0)
				return ;
		}
		if (!ss.eof()) {
			std::streampos	body_start = ss.tellg();
			if (static_cast<int>(body_start) == -1) {
				Logger::log(E_ERROR, COLOR_RED, "Request::add error parsing request.");
				this->status_code_ = E_INTERNAL_SERVER_ERROR;
			}
			else {
				if (static_cast<int>(body_start) != static_cast<int>(bytes_read))
					saveBody_(string_add, static_cast<int>(body_start), bytes_read);
				if (this->status_code_ < 400 && !this->raw_body_.empty())
					parseBody_();
			}
		}
		if (this->headers_complete && this->body_size_ == this->body_len_received_) {
			this->complete_ = true;
		}
	}
	catch (const std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "Request::add caught exception: %s", e.what());
		this->status_code_ = E_INTERNAL_SERVER_ERROR;
	}
}

/*! \brief clears all containers and resets all values to intial state
*       
*	All attributes are cleared so client can reuse the object for the next request.
*/
void	Request::clear( void ) {

	this->body_size_ = 0;
	this->body_len_received_ = 0;
	this->chunked_ = false;
	this->keep_alive_ = false;
	this->cgi_flag_ = false;
	this->headers_complete = false;
	this->request_line_.clear();
	this->headers_.clear();
	this->raw_body_ = "";
	this->processed_body_ = "";
	this->body_vector_.clear();
	this->complete_ = false;
	this->file_upload_ = false;
	this->file_mime_ = "";
	this->status_code_ = E_UNSET;
	this->file_content_ = "";
	this->file_name_ = "";
	this->query_encode_ = false;
}

/*! \brief prints to standard output `REQUEST:` followed by
*			each request element on a newline.
*
*	Prints to standard output `REQUEST:` followed by each request element on a newline.
*	Each version of the body that is stored is printed or indicates that it is empty.
*/
void	Request::printRequest( void ) const {

	std::cout << "REQUEST: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->request_line_.begin(); it != this->request_line_.end(); it++) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "\nHeaders:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); it++) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "\nBody raw:" << std::endl;
	if (!this->raw_body_.empty()) {
		std::cout << this->raw_body_;
		std::cout << std::endl;
	}
	else {
		std::cout << "no body" << std::endl;
	}
	std::cout << "\nBody Processed:" << std::endl;
	if (!this->processed_body_.empty()) {
		std::cout << this->processed_body_;
		std::cout << std::endl;
	}
	else {
		std::cout << "no body" << std::endl;
	}
	std::cout << "\nFile Content:" << std::endl;
	if (!this->file_content_.empty()) {
		std::cout << this->file_content_;
		std::cout << std::endl;
	}
	else {
		std::cout << "no file content" << std::endl;
	}
}

/************** PUBLIC GETTERS **************/

/*! \brief returns bool indicating if the body should be url encoded if saved to a query string
*
*	Returns bool indicating if the body should be url encoded if saved to a query string.
*	by default it is false and set to true in parseBody_ if multipart form is present.
*  
*	@return @b bool indicating if the body should be url encoded if saved to a query string
*/
bool	Request::getQueryEncode( void ) const {

	return this->query_encode_;
}

/*! \brief returns const reference to the hostname from the Host header
* 
*	@return @b const @b std::string& of the hostname from the Host header
*/
const std::string&	Request::getRequestHostName( void ) const {

	return this->host_name_;
}

/*! \brief returns int of request's port number
*
*	@return @b int representing the request's port number
*/
int	Request::getRequestPort( void ) const {

	return this->port_;
}

/*! \brief returns bool indicating if this request is for a cgi script
*
*	Returns bool indicating if this request is for a cgi script.
*
*	@return @b bool that indicates if the request is for a cgi script 
*/
bool	Request::getCgiFlag( void ) const {

	return this->cgi_flag_;
}

/*! \brief returns size_t of the body size indicated by the request header
*  
*	@return @b size_t of the body size indicated by the request header
*/
size_t		Request::getBodySize( void ) const {
	
	return this->body_size_;
}

/*! \brief returns size_t of the body size actually received
*
*	@return @b size_t of the body size actually received so far
*/
size_t		Request::getBodyLengthReceived( void ) const {

	return this->body_len_received_;
}

/*! \brief returns bool indicating if the Transfer-Encoding = chunked
*
*	Returns bool indicating if request body is chunked based on the 
*	value of the Transfer-Encoding header;
*  
*	@return @b bool indicating if the request body is chunked based on Transfer_encoding header
*/
bool	Request::getChunked( void ) const {

	return this->chunked_;
}

/*! \brief returns bool indicating requests Connetion header value for keep alive
*
*	Returns bool indicating request's Connection header value for keep alive.
* 	@return @b bool indicating if client connection is to be kept alive after a response is sent
*/
bool	Request::getKeepAlive( void ) const {

	return this->keep_alive_;
}

/*! \brief returns bool indicating if request is completed
*
*	Returns bool indicating if the request is considered complete.
*	This is based on all headers and promised body length received.
*  
*	@return @b bool indicating if the request iis complete or not
*/
bool	Request::getComplete( void ) const {

	return this->complete_;
}

/*! \brief returns the request line value for the key passed
*
*	Returns request line value for key passed as parameter. If key is not in
*	request line map, an empty string is returned.
*  
*	@return @b std::string request line value for key passed as parameter
*/
std::string	Request::getRequestLineValue( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->request_line_.find(key);
	if (value ==  this->request_line_.end()) {
		return "";
	}
	else {
		return value->second;
	}
}

/*! \brief returns a const_iterator to the begining of request headers map
*
*	Returns a const_iterator to the request headers using the map begin() method.
*
*  @return @b std::map<std::string, @b std::string>::const_iterator to the request headers using the map begin() method
*/
std::map<std::string, std::string>::const_iterator	Request::getHeaderBegin( void ) const {

	return this->headers_.begin();
}

/*! \brief returns a const_iterator to the end of the request headers map
*
*	Returns a const_iterator to the request headers using the map end() method.
*
*	@return @b std::map<std::string, @b std::string>::const_iterator to the request headers using the map end() method
*/
std::map<std::string, std::string>::const_iterator	Request::getHeaderEnd( void ) const {

	return this->headers_.end();
}

/*! \brief returns header value as std::string for header name passed as key
*
*	Returns header value as std::string for header name passed as key. If header is not
*	in request, an empty string is returned.
*  
*	@return @b std::string of header value for header name passed as key
*/
std::string	Request::getHeaderValueByKey( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->headers_.find(key);
	if (value ==  this->headers_.end()) {
		return "";
	}
	else {
		return value->second;
	}
}

/*! \brief returns a const reference to the procesed body string
*
*	Returns a const reference to the processed body string. Will not include any 
*	file contents that were submitted by multipart form.
*
*	@return @b const @b std::string& of the processed body string
*/
const std::string&		Request::getProcessedBody( void ) const {

	return this->processed_body_;
}

/*! \brief returns a const reference to the file content for upload
*
*	Returns a const reference to the file content for upload that is stored from
*	a multipart form. The file name can be obtained from getUploadName().
*
*	@return @b const @b std::string& of file content for upload
*/
const std::string&		Request::getUploadContent( void ) const {

	return this->file_content_;
}

/*! \brief returns a const reference to the file name for upload
*
*	Returns a const reference to the file name for upload that is stored from
*	a multipart form. The file content can be obtained from getUploadContent().	
*  
*	@return @b const @b std::string& of file name for upload
*/
const std::string&		Request::getUploadName( void ) const {

	return this->file_name_;
}

/*! \brief returns bool indicating if a file has been stored for upload
*
*	Returns bool indicating if a file has been stored for upload. File name and content
*	can be obtained from getUploadName() and getUploadContent().
*  
@	@return bool indicating if a file has been stored for upload
*/
bool				Request::isFileUpload( void ) const {

	return this->file_upload_;
}

/*! \brief returns the http status code that indicates the status for response based on 
*				processing of body and headers in request class.
*
*	Returns unsigned int indicating the status code that may be set to an error based on
*	the header checks and body processing that occurs in the Request class. It is intialized
*	to 0 which indicates no errors. Any other value may indicate an error encountered.
*  
*	@return @b unsigned @b int indicating the status code
*/
unsigned int		Request::getStatusCode( void ) const {

	return this->status_code_;
}

/*! \brief returns const reference to the upload mime type
*
*	Returns a const reference to the mime type for the upload file indicated
*	in the header in the multipart form.
*
*	@return @b const @b std::string& of upload mime type
*/
const std::string&		Request::getUploadMime( void ) const {

	return this->file_mime_;
}

/*! \brief returns bool if request timed out based on REQUEST_TIMEOUT_SEC macro
*
*	Returns bool if request timed out based on the REQUEST_TIMEOUT_SEC macro.
*	Calculated from the current time and the time response processing started.
*  
*	@return @b bool indicating if request has timed out or not
*/
bool	Request::checkRequestTimeout( void ) const {

	time_t	current_time = time(NULL);
	double	time_since_latest_action = difftime(current_time, this->request_start_time_);
	
	if (time_since_latest_action >= REQUEST_TIMEOUT_SEC) {
		Logger::log(E_INFO, COLOR_BRIGHT_BLUE, "request for uri: %s timed out!", getHeaderValueByKey("uri").c_str());
		return true;
	}
	return false;
}

/************** PUBLIC SETTERS **************/

/*! \brief public setter for the cgi flag take new bool value
*
*	Public setter for the cgi flag to allow response to change value of cgi bool
*	if redirection or alias points to cgi script.
*/
void	Request::setCgiFlag( bool flag) {
	
	this->cgi_flag_ = flag;
}

/*! \brief public function to clear upload content
*
*	Clears the upload content. Intended to be used after a temp file has been created.
*/
void	Request::clearUploadContent( void ) {

	this->file_content_ = "";	
}

/************** CLASS PRIVATE METHODS **************/

/************** PRIVATE SETTERS **************/

/*! \brief private setter for body_length based on request headers
*
*	Private setter for body_length based on request Content-Length Header.
*	If no Content-Length header is present, the body size is set to 0.
*/
void	Request::setBodySize_( void ) {

	std::string	content_length = this->headers_["Content-Length"];
	if (content_length.empty()) {
		this->body_size_ = 0;
	}
	else {
		try {
			this->body_size_ = std::stod(content_length);
		}
		catch (std::exception& e){
			Logger::log(E_ERROR, COLOR_RED, "Request body size overflowed on conversion.");
			this->status_code_ = E_PAYLOAD_TOO_LARGE;
		}
	}
}

/*! \brief private setter for chunked bool based on request headers
*
*	Private setter for chunked bool based on Transfer-Encoding header.
*/
void	Request::setChunked_( void ) {

	std::string	transfer_encoding = this->headers_["Transfer-Encoding"];
	if (transfer_encoding == "chunked") {
		this->chunked_ = true;
	}
	else {
		this->chunked_ = false;
	}
}

/*! \brief private setter for keep_alive_ bool based on request headers
*
*	Private setter for keep_alive_ bool based on Connection header.
*/
void	Request::setKeepAlive_( void ) {

	std::string connection = this->headers_["Connection"];
	if (connection == "keep-alive") {
		this->keep_alive_ = true;
	}
	else {
		this->keep_alive_ = false;
	}
}

/*! \brief private setter for the cgi flag. Sets bool based on uri
*
*	Private setter for the cgi flag.
*	Sets bool true if uri begins with `/cgi-bin'.
*/
void	Request::setCgiFlag_( void ) {

	std::string	uri = this->getRequestLineValue("uri");
	if (strncmp(uri.c_str(), "/cgi-bin", 8) == 0) {
		this->cgi_flag_ = true;
	}
	else {
		this->cgi_flag_ = false;
	}
}

/*! \brief private setter for the host name and port based on Host Header
*
*	Private setter for the the host name and port based on Host Header.
*	If the host header os empty the status code will be set to E_BAD_REQUEST.
*/
void	Request::setHostNameAndPort_( void ) {

	std::string	host_header = getHeaderValueByKey("Host");
	if (host_header.empty()) {
		this->status_code_ = E_BAD_REQUEST;
		return ;
	}
	std::string	request_host_name;
	std::string request_port;
	int	last_colon_pos = host_header.find_last_of(':');
	this->host_name_ = host_header.substr(0, last_colon_pos);
	this->port_ = ft_stoi(host_header.substr(last_colon_pos + 1));
	Logger::log(E_DEBUG, COLOR_BRIGHT_BLUE, "Request Host Name: %s, Request Port: %d", this->host_name_.c_str(), this->port_);
}

/*! \brief calls all private setters to intialize request attibutes based on headers
*
*	Calls all private setters to intialize request attibutes based on headers.
*/
void	Request::setRequestAttributes_( void ) {

	void	(Request::*setters[])(void) = { &Request::setKeepAlive_, &Request::setChunked_, &Request::setBodySize_ , &Request::setCgiFlag_, &Request::setHostNameAndPort_ };
	for (int i = 0; i < 5; i++) {
		(this->*setters[i])();
	}
}

/*! \brief stores request line values in map
*
*	Stores request line values in map. if method not implemented or HTTP version
*	is not supported, the status code will be set and the parsing stops.
*	The uri as received is stored under "raw_uri" and the url decoded uri is stored
*	under "uri" in the request_line_ map.
*/
void	Request::parseRequestLine_( std::string& to_parse ) {

	std::stringstream	ss(to_parse);
	std::string			part;

	if (ss.fail() || ss.bad()) {
		throw(std::runtime_error("Stringstream failed in parseRequestLine_"));
	}
	ss >> part;
	if (part == "GET" || part == "HEAD" || part == "POST" || part == "DELETE") {
		this->request_line_["method"] = part;
	}
	else {
		this->status_code_ = E_NOT_IMPLEMENTED;
		return ;
	}
	ss >> part;
	this->request_line_["raw_uri"] = part;
	this->request_line_["uri"] = urlDecode(part);
	ss >> part;
	if (part != "HTTP/1.1") {
		this->status_code_ = E_HTTP_VERSION_NOT_SUPPORTED;
		return ;
	}
	this->request_line_["version"] = part;
}

/*! \brief parses key and value from request header
*       
*
*  If a header already exists, it will not overwrite the old value.
*/
void	Request::parseHeader_( std::string& to_parse ) {

	std::stringstream	ss(to_parse);
	std::string			key;
	std::string			value;
	size_t				key_len;

	if (ss.fail() || ss.bad()) {
		throw(std::runtime_error("Stringstream failed in parseHeader_"));
	}
	getline(ss, key, ':');
	key_len = key.length();
	value = to_parse.substr(key_len + 2, to_parse.length() - key_len);
	value.pop_back();
	if (this->headers_[key].empty()) {
		this->headers_[key] = value;
	}
}

/*! \brief saves request body into raw_body_ and adds the length to body_len_received
*
*	Saves request body into raw_body_ character by character to ensure entire message
*	(including any binary file content) is transfered and adds the length to 
*	body_len_recieved_.
*/
void Request::saveBody_(std::string& to_add, size_t body_start, size_t total_bytes) {
	
	if ( body_start == to_add.size()) {
		this->body_len_received_ = 0;
		return ;
	}
	else {
		size_t	body_length = total_bytes - body_start;
		size_t	body_index = body_start;

		this->body_vector_.clear();
		for (; body_index < to_add.size(); ++body_index) {
			this->body_vector_.push_back(to_add[body_index]);
		}
		if (body_length != body_index - body_start) {
			this->status_code_ = E_BAD_REQUEST;
			return ;
		}
		this->raw_body_.append(this->body_vector_.begin(), this->body_vector_.end());
		this->body_len_received_ += body_length;
	}
}

/*! \brief static function that parses boundary from the Content-Type header
*
*	Static function parses boundary from the Content-Type header for multipart form.
*/
static std::string	parseBoundary(std::string& content_type_header ) {

	int			boundry_start = content_type_header.find(";") + 1;
	std::string	boundary = content_type_header.substr(boundry_start);
	std::string	boundary_id = " boundary=";
	if (boundary.size() < boundary_id.size())
		return "";
	boundary.erase(0, boundary_id.length());
	Logger::log(E_DEBUG, COLOR_BRIGHT_MAGENTA, "boundry from header : %s, parsed: %s", content_type_header.c_str(), boundary.c_str());
	return boundary;
}

/*! \brief directs parsing of raw_body_ into processed_body_ based on the content type
*
*	Directs parsing of raw_body_ based on the content type (chunked, multipart/form-data, or other)
*	If no boundary found for multipart/form-data, the request is rejected. 
*	According to RFC 7231 "the definition of GET has been relaxed so that requests can have a body, 
*	even though a body has no meaning for GET", therefore the request is not rejected based on method.
*/
void	Request::parseBody_( void ) {

	std::string content_type_header = getHeaderValueByKey("Content-Type");
	bool		is_multipart_form = content_type_header.find("multipart/form-data") != std::string::npos ? true : false;

	if (this->chunked_) {
		parseChunkedBody_();
	}
	else if (this->body_size_ != this->body_len_received_) {
		return ;
	}
	else if (is_multipart_form) {
		std::string	boundry = parseBoundary(content_type_header);
		if (boundry.empty()) {
			this->status_code_ = E_BAD_REQUEST;
			return ;
		}
		parseMultipartForm_(boundry);
		query_encode_ = true;
	}
	else {
		this->processed_body_.append(this->raw_body_);
	}
}

/*! \brief parses chunked body, setting chunked value to false if all chunks are processed
*
*	Parses chunked body setting chunked value to false if all chunks are processed. Each
*	loop process one chunk and may process multiple chunks if they are present in the same message.
*	If invalid formatting is found, the status code is set to 400/Bad Request. If an conversion for
*	the hexadecimal to decimal fails, 500/Internal Server Error is set.
*	When end of chunked data is found, the chunked flag is dropped and the bodysize is set to 
*	the size of the total body received as the Content-Size header is not present for 
*	chunked encoding.
*/
void	Request::parseChunkedBody_( void ) {

	std::string		parse_buffer = "";
	size_t			body_index = 0;
	long int		convertedLength = 0;
	

	while (body_index < this->raw_body_.size()) {
		while (parse_buffer.back() != '\n') {
			parse_buffer += this->raw_body_[body_index];
			body_index++;
		}
		if ((parse_buffer.size() > 1 && parse_buffer[parse_buffer.size() - 2] != '\r') || !isxdigit(parse_buffer[0])) {
			this->status_code_ = E_BAD_REQUEST;
			return ;
		}
		else if (parse_buffer == "0\r\n") {
			this->chunked_ = false;
			this->body_size_ = this->body_len_received_;
			this->raw_body_.clear();
			break ;
		}
		else {
			std::istringstream	converter(parse_buffer);
			if (!(converter >> std::hex >> convertedLength)) {
				this->status_code_ = E_INTERNAL_SERVER_ERROR;
				throw std::runtime_error("Chunked data: string conversion error:");
			}
			parse_buffer.clear();
			while (convertedLength && body_index < this->raw_body_.size()) {
				parse_buffer += this->raw_body_[body_index];
				body_index++;
				convertedLength--;
			}
			if (convertedLength != 0) {
				this->status_code_ = E_BAD_REQUEST;
				return ;
			}
			else {
				this->processed_body_.append(parse_buffer);
				parse_buffer.clear();
				body_index += 2;
			}
		}
	}
}

/*! \brief stores file contents from multipart form data
*
*	Stores file contents from multipart/form-data, stopping once a boundary is found.
*	The last two characters are removed as they are the CRLF formatting and not
*	part of the file contents.
*/
void	Request::storeFileContents_( const std::string& section_bound, const std::string& last_bound, size_t& body_index ) {


	std::string 	parse_buffer = "";

	while (body_index < this->raw_body_.size()) {
		while (parse_buffer.back() != '\n') {
			parse_buffer += this->raw_body_[body_index];
			body_index++;
		}
		if (parse_buffer == section_bound || parse_buffer == last_bound) {
			this->file_content_.erase(this->file_content_.end() - 2, this->file_content_.end());
			break ;
		}
		else {
			this->file_content_.append(parse_buffer);
			parse_buffer.clear();
		}
	}
}

/*! \brief sets the filename indicated in the from data header and sets file_upload to true
*
*	Sets the filename indicated in the from data header. Removes quotes 
*	surrounding the filename. Sets file_upload bool to true.
*/
void	Request::setFilename_( const std::string& to_parse ) {

	int fname_start = to_parse.find("filename=") + 9;

	std::string fname = to_parse.substr(fname_start);
	fname.erase(0,1);
	fname.erase(fname.length() - 3, 3);
	this->file_name_ = fname;

	this->file_upload_ = true;
}

/*! \brief parses multipart/form-data, removing boundaries and saving file content 
*			separately from processed_body_
*
*	Parses multipart/form-data by Removing boundaries. Non-file content is added to 
*	processed_body_, including CRLF formatting, and prepending "value=" to form field
*	values that are not files.
*	File content is processed and stored separately by storeFileContents_().
*/
void	Request::parseMultipartForm_( std::string boundary ) {

	std::string 	section_bound = "--" + boundary;
	std::string		last_bound = section_bound + "--\r\n"; section_bound.append("\r\n");
	std::string 	parse_buffer = "";
	size_t			body_index = 0;
	bool			file_present = false;

	while (body_index < this->raw_body_.size()) {
		parse_buffer += this->raw_body_[body_index++];
		if (parse_buffer.back() == '\n' && parse_buffer.size() > 1 && parse_buffer[parse_buffer.size() - 2] == '\r') {
			if (parse_buffer == CRLF) {
				if (file_present) {
					storeFileContents_(section_bound, last_bound, body_index);//next parse field contents ;
					file_present = false;
				}
				else {
					this->processed_body_.append("value=");
				}
			}
			else if (parse_buffer == last_bound)
				break ;
			else if (parse_buffer != section_bound) {
				if (parse_buffer.find("filename=") != std::string::npos) {
					setFilename_(parse_buffer);
					file_present = true;
				}
				if (file_present && strncmp(parse_buffer.c_str(), "Content-Type:", 13) == 0) {
					this->file_mime_ = parse_buffer.substr(14, parse_buffer.size() - 2);
				}
				this->processed_body_.append(parse_buffer);
			}
			parse_buffer.clear();
		}
	}
}
