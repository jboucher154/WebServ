
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
sever_error_(false), 
raw_body_(""), 
processed_body_(""),
file_content_(""),
file_name_(""),
body_vector_(),
file_upload_(false),
file_mime_(""),
status_code_(0) {

	/* default constructor */
}

/*! \brief copy constructor calls copy assignment operator
*
*	
*  Copy constructor calls copy assignment operator.
*  
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
*	
*  Copy assignment operator copies all values from rhs.
*  
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
		this->cgi_flag_ = rhs.cgi_flag_;
		this->file_upload_ = rhs.file_upload_;
		this->file_mime_ = rhs.file_mime_;
		this->status_code_ = rhs.status_code_;
		this->file_content_ = rhs.file_content_;
		this->file_name_ = rhs.file_name_;
	}
	return (*this);
}

/************** CLASS PUBLIC METHODS **************/

/*! \brief add to any part of the Request, directs to correct private parsing function
*       
*	Character buffer is converted to string and string stream to process into the relevant
*	request part: request line, header, or body. Request attributes are set and checked
*	based on headers then body is saved and parsed nusing the total bytes read from the 
*	recv() call to guide the parsing. If exception for string conversions occur they are 
*	caught and the error printed.
*    
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
			}
			else {
				this->parseHeader_(line);
			}
			if (this->status_code_ > 0)
				return ; //stop processing if error found
		}
		this->setRequestAttributes();
		//process body
		if (!ss.eof()) {
			std::streampos	body_start = ss.tellg();
			if (static_cast<int>(body_start) == -1) {
				this->sever_error_ = true;
			}
			else {
				if (static_cast<int>(body_start) != static_cast<int>(bytes_read))
					saveBody_(string_add, static_cast<int>(body_start), bytes_read);
				if (this->status_code_ < 400 && !this->raw_body_.empty() && this->body_size_ == this->body_len_received_)
					parseBody_();
			}
		}
	}
	catch (const std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "Request::add caught exception: %s", e.what());
		this->sever_error_ = true;
	}
	std::cout << "*** BODY LEN VS RECEIVED [add] : " << this->body_size_ << " vs. " << this->body_len_received_ << std::endl;
	printRequest();//debugging
}

/*! \brief clears all containers and resets all values to intial state
*       
*	All attributes are cleared so client can reuse the object for the next request.
*  
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
	this->sever_error_ = false;
	this->file_upload_ = false;
	this->file_mime_ = "";
	this->status_code_ = 0;
	this->file_content_ = "";
	this->file_name_ = "";
}

/*! \brief prints to standard output `REQUEST:` followed by
*				each request element on a newline.
*
*	Prints to standard output `REQUEST:` followed by each request element on a newline.
*	Each version of the body that is stored is printed or indicates that it is empty.
*  
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

/*! \brief returns bool indicating if this request is for a cgi script
*
*	Returns bool indicating if this request is for a cgi script.
*  
*/
bool	Request::getCgiFlag( void ) const {

	return (this->cgi_flag_);
}

/*! \brief returns size_t of the body size indicated by the request header
*
*	Returns size_t of the body size indicated by the request header
*  
*/
size_t		Request::getBodySize( void ) const {
	
	return (this->body_size_);
}

/*! \brief returns size_t of the body size actually received
*
*	Returns size_t of the body size actually received so far.
*  
*/
size_t		Request::getBodyLengthReceived( void ) const {

	return (this->body_len_received_);
}

/*! \brief returns bool indicating if the Transfer-Encoding = chunked
*
*	Returns bool indicating if request body is chunked based on the 
*	value of the Transfer-Encoding header;
*  
*/
bool	Request::getChunked( void ) const {

	return (this->chunked_);
}

/*! \brief returns bool indicating requests Connetion header value for keep alive.
*
*	rRturns bool indicating requests Connetion header value for keep alive.
*  
*/
bool	Request::getKeepAlive( void ) const {

	return (this->keep_alive_);
}

/*! \brief returns bool indicating if request is completed
*
*	Returns bool indicating if the request is considered complete.
*	This is based on all headers and promised body length received.
*  
*/
bool	Request::getComplete( void ) const {

	return (this->complete_);
}

/*! \brief returns bool if there was a server error
*
*	Returns bool if there was a server error while processing the request.
*  
*/
bool	Request::getServerError( void ) const {
	
	return (this->sever_error_);
}

/*! \brief returns the request line value for the key passed
*
*	Returns request line value for key passed as parameter. If key is not in
*	request line map, an empty string is returned.
*  
*/
std::string	Request::getRequestLineValue( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->request_line_.find(key);
	if (value ==  this->request_line_.end()) {
		return ("");
	}
	else {
		return (value->second);
	}
}

/*! \brief returns a const_iterator to the begining of request headers map
*
*	Returns a const_iterator to the request headers using the map begin() method.
*  
*/
std::map<std::string, std::string>::const_iterator	Request::getHeaderBegin( void ) const {

	return (this->headers_.begin());
}

/*! \brief returns a const_iterator to the end of the request headers map
*
*	Returns a const_iterator to the request headers using the map end() method.
*  
*/
std::map<std::string, std::string>::const_iterator	Request::getHeaderEnd( void ) const {

	return (this->headers_.end());
}

/*! \brief returns header value as std::string for header name passed as key
*
*	Returns header value as std::string for header name passed as key. If header is not
*	in request, an empty string is returned.
*  
*/
std::string	Request::getHeaderValueByKey( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->headers_.find(key);
	if (value ==  this->headers_.end()) {
		return ("");
	}
	else {
		return (value->second);
	}
}

/*! \brief returns a const reference to the procesed body string
*
*	Returns a const reference to the processed body string. Will not include any 
*	file contents that were submitted by multipart form.
*  
*/
const std::string&		Request::getProcessedBody( void ) const {

	return this->processed_body_;
}

/*! \brief returns a const reference to the file content for upload
*
*	Returns a const reference to the file content for upload that is stored from
*	a multipart form. The file name can be obtained from getUploadName().
*  
*/
const std::string&		Request::getUploadContent( void ) const {

	return this->file_content_;
}

/*! \brief returns a const reference to the file name for upload
*
*	Returns a const reference to the file name for upload that is stored from
*	a multipart form. The file content can be obtained from getUploadContent().	
*  
*/
const std::string&		Request::getUploadName( void ) const {

	return this->file_name_;
}

/*! \brief returns bool indicating if a file has been stored for upload
*
*	Returns bool indicating if a file has been stored for upload. File name and content
*	can be obtained from getUploadName() and getUploadContent().
*  
*/
bool				Request::isFileUpload( void ) const {

	return this->file_upload_;
}

/*! \brief returns the http status code that indicates the status for response based on 
*				processing of body and headers in request class.
*
*	Returns unsigned int indicating the status code thatt may be set to an error based on
*	the header checks and body processing thatt occurs in the request class. It is intialized
*	to 0, which indicates no errors. Any other value may indicate an error encountered.
*  
*/
unsigned int		Request::getStatusCode( void ) const {

	return this->status_code_;
}

/*! \brief returns const reference to the upload mime type
*
*	Returns a const reference to the mime type for the upload file indicated
*	in the header in the multipart form.
*  
*/
const std::string&		Request::getUploadMime( void ) const {

	return this->file_mime_;
}

/************** PUBLIC SETTERS **************/

/*! \brief public setter for the cgi flag take new bool value
*
*	Public setter for the cgi flag to allow response to change value of cgi bool
*	if redirection or alias points to cgi script.
*  
*/
void	Request::setCgiFlag( bool flag) {
	
	this->cgi_flag_ = flag;
}

/************** CLASS PRIVATE METHODS **************/

/************** PRIVATE SETTERS **************/

/*! \brief private setter for body_length based on request headers
*
*	Private setter for body_length based on request Content-Length Header.
*	If no Content-Length header is present the body size is set to 0.
*  
*/
void	Request::setBodySize( void ) {

	std::string	content_length = this->headers_["Content-Length"];
	if (content_length.empty()) {
		this->body_size_ = 0;
	}
	else {
		try {
			this->body_size_ = ft_stoi(content_length);
		}
		catch (std::exception& e){
			Logger::log(E_ERROR, COLOR_RED, "Request body size overflowed on conversion.");
			//413 content too large
		}
	}
}

/*! \brief private setter for chunked bool based on request headers
*
*	Private setter for chunked bool based on Transfer-Encoding header.
*  
*/
void	Request::setChunked( void ) {

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
*  
*/
void	Request::setKeepAlive( void ) {

	std::string connection = this->headers_["Connection"];
	if (connection == "keep-alive") {
		this->keep_alive_ = true;
	}
	else {
		this->keep_alive_ = false;
	}
}

/*! \brief 
*
*	
*  
*/
void	Request::setCgiFlag( void ) {

	std::string	uri = this->getRequestLineValue("uri");
	if (strncmp(uri.c_str(), "/cgi-bin", 8) == 0) {
		this->cgi_flag_ = true;
	}
	else {
		this->cgi_flag_ = false;
	}
}

/*! \brief calls all private setters to intialize request attibutes based on headers
*
*	Calls all private setters to intialize request attibutes based on headers.
*  
*/
void	Request::setRequestAttributes( void ) {

	void	(Request::*setters[])(void) = { &Request::setKeepAlive, &Request::setChunked, &Request::setBodySize , &Request::setCgiFlag };
	for (int i = 0; i < 4; i++) { //get size of setters instead of 3
		(this->*setters[i])();
	}
}

/*! \brief stores request line values in map
*
*	Stores request line values in map. if method not implemented or HTTP version
*	is not supported the status code will be set and the parsing stops.
*  
*/
void	Request::parseRequestLine_( std::string& to_parse ) {

	std::stringstream	ss(to_parse);
	std::string			part;

	if (ss.fail() || ss.bad()) {
		throw(std::runtime_error("Stringstream failed in parseRequestLine_"));
	}
	ss >> part;
	if (part != "GET" || part != "HEAD" || part != "POST" || part != "DELETE") {
		this->status_code_ = 501; //not implemented
		return ;
	}
	this->request_line_["method"] = part;
	ss >> part;
	this->request_line_["uri"] = part;
	ss >> part;
	if (part != "HTTP/1.1") {
		this->status_code_ = 505; //HTTP version not supported
		return ;
	}
	this->request_line_["version"] = part;
}

/*! \brief parses key and value from request header.
*       
*
*  If a header already exists it will not overwrite the old value.
*  
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

/*! \brief saves request body into raw_body_ and adds the length to body_len_recieved
*
*	Saves request body into raw_body_ character by character to ensure entire message
*	(including any binary file content) is transfered and adds the length to 
*	body_len_recieved_.
*  
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
			this->status_code_ = 400;
			return ;
		}
		this->raw_body_.append(this->body_vector_.begin(), this->body_vector_.end());
		this->body_len_received_ += body_length;
	}
}

/*! \brief static function that parses boundary from the Content-Type header
*
*	Static function parses boundary from the Content-Type header for multipart form.
*  
*/
static std::string	parseBoundry(std::string& content_type_header ) {

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
*	If no boundary found for multipart/form-data the request is rejected.
*  
*/
void	Request::parseBody_( void ) {
	//check if body present with GET request
	//check body type (only plain for now)
	std::string content_type_header = getHeaderValueByKey("Content-Type");
	bool		is_multipart_form = content_type_header.find("multipart/form-data") != std::string::npos ? true : false;

	if (this->chunked_) {
		parseChunkedBody_();
	}
	else if (is_multipart_form) {
		std::string	boundry = parseBoundry(content_type_header);
		if (boundry.empty()) {
			this->status_code_ = 400; //no boundry provided, invalid request
			return ;
		}
		parseMultipartForm_(boundry);
	}
	else {
		this->processed_body_.append(this->raw_body_, this->raw_body_.size());
	}
}

/*! \brief parses chunked body setting chunked value to false if all chunks are processed
*
*	Parses chunked body setting chunked value to false if all chunks are processed. Each
*	loop process one chunk and may process multiple chunks if they are present in the same message.
*	If invalid formatting is found the status code is set to 400/Bad Request. If an conversion for
*	the hexadecimal to decimal fails 500/Internal Server Error is set.
*  
*/
void	Request::parseChunkedBody_( void ) {

	std::string		parse_buffer = "";
	size_t			body_index = 0;
	long int		convertedLength = 0;
	

	while (body_index < this->raw_body_.size()) {
		//get first line that will have hex number followed by CRLF
		while (parse_buffer.back() != '\n') {
			parse_buffer += this->raw_body_[body_index];
			body_index++;
		}
		if ((parse_buffer.size() > 1 && parse_buffer[parse_buffer.size() - 2] != '\r') || !isxdigit(parse_buffer[0])) {
			this->status_code_ = 400; //bad request, incorrect format
			return ;
		}
		else if (parse_buffer == "0\r\n") { //end of chunks
			this->chunked_ = false; //? or set another value to see that all chunks processed?
			break ;
		}
		else {
			//convert hex number to decimal
			std::istringstream	converter(parse_buffer);
			if (!(converter >> std::hex >> convertedLength)) {
				this->status_code_ = 500; //conversion error
				return ;
			}
			parse_buffer.clear();
			while (convertedLength && body_index < this->raw_body_.size()) {
				parse_buffer += this->raw_body_[body_index];
				body_index++;
				convertedLength--;
			}
			if (convertedLength != 0) {
				this->status_code_ = 400; //bad request, chunk not length indicated
				return ;
			}
			else {
				this->processed_body_.append(parse_buffer);
				parse_buffer.clear();
				body_index += 2; //move past CRLF
			}
		}
	}
}

/*! \brief stores file contents from multipart form data
*
*	Stores file contents from multipart/form-data, stopping once a boundary is found.
*	The last two characters are removed as they are the CRLF formatting and not
*	part of the file contents.
*  
*/
void	Request::storeFileContents_( const std::string& section_bound, const std::string& last_bound, size_t& body_index ) {


	std::string 	parse_buffer = "";

	while (body_index < this->raw_body_.size()) {
		while (parse_buffer.back() != '\n') {
			parse_buffer += this->raw_body_[body_index];
			body_index++;
		}
		if (parse_buffer == section_bound || parse_buffer == last_bound) {
			this->file_content_.erase(this->file_content_.end() - 2, this->file_content_.end());//remove the last CRLF
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
*  
*/
void	Request::setFilename( const std::string& to_parse ) {

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
*	processed_body_, including CRLF formatting, and prepending "value=" to form feild
*	values that are not files.
*	File content is procesed and stored separatley by storeFileContents_().
*
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
					setFilename(parse_buffer);
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
