
#include "Request.hpp"

/* CONSTRUCTORS */

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

Request::Request( const Request& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

Request::~Request( void ) {

	/* destructor */
} 

/* OPERATOR OVERLOADS */

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
*
*  More details to be filled as project progresses.
*  
*/
void	Request::add( char* to_add, size_t bytes_read ) {
	
	// std::cout << "*** raw body from recv [add] : " << to_add << std::endl;
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
		if (this->headers_complete && this->body_size_ == this->body_len_received_) {
			this->complete_ = true;
			//TESTING !!!!!!!
			if (!this->file_content_.empty()) {
				std::ofstream	newfile(this->file_name_, std::ofstream::binary);
				newfile << this->file_content_;
				newfile.close();
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
*
*  More details to be filled as project progresses.
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

/*! \brief prints to standard output `REQUEST` followed by
*				each request element on a newline.
*
*  More details to be filled as project progresses.
*  
*/
void	Request::printRequest( void ) const {

	std::cout << "REQUEST: " << std::endl;
	// for (std::map<std::string, std::string>::const_iterator it = this->request_line_.begin(); it != this->request_line_.end(); it++) {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }
	// std::cout << "\nHeaders:" << std::endl;
	// for (std::map<std::string, std::string>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); it++) {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }
	std::cout << "\nBody raw:" << std::endl;
	if (!this->raw_body_.empty()) {
		std::cout << this->raw_body_;
		std::cout << std::endl;
	}
	std::cout << "\nBody Processed:" << std::endl;
	if (!this->processed_body_.empty()) {
		std::cout << this->processed_body_;
		std::cout << std::endl;
	}
	std::cout << "\nFile Content:" << std::endl;
	if (!this->file_content_.empty()) {
		std::cout << this->file_content_;
		std::cout << std::endl;
	}
	// if (!this->body_vector_.empty()) {
	// 	for (std::vector<u_int8_t>::const_iterator it = this->body_vector_.begin(); it != this->body_vector_.end(); it++) {
	// 		std::cout << *it;;
	// 	}
	// 	std::cout << std::endl;
	// }
}

/************** PUBLIC GETTERS **************/

bool	Request::getCgiFlag( void ) const {

	return (this->cgi_flag_);
}

size_t		Request::getBodySize( void ) const {
	
	return (this->body_size_);
}

size_t		Request::getBodyLengthReceived( void ) const {

	return (this->body_len_received_);
}

bool	Request::getChunked( void ) const {

	return (this->chunked_);
}

bool	Request::getKeepAlive( void ) const {

	return (this->keep_alive_);
}

bool	Request::getComplete( void ) const {

	return (this->complete_);
}

bool	Request::getServerError( void ) const {
	
	return (this->sever_error_);
}

std::string	Request::getRequestLineValue( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->request_line_.find(key);
	if (value ==  this->request_line_.end()) {
		return ("");
	}
	else {
		return (value->second);
	}
}

std::map<std::string, std::string>::const_iterator	Request::getHeaderBegin( void ) const {

	return (this->headers_.begin());
}

std::map<std::string, std::string>::const_iterator	Request::getHeaderEnd( void ) const {

	return (this->headers_.end());
}

std::string	Request::getHeaderValueByKey( std::string key ) const {

	std::map<std::string, std::string>::const_iterator value = this->headers_.find(key);
	if (value ==  this->headers_.end()) {
		return ("");
	}
	else {
		return (value->second);
	}
}

std::string::iterator	Request::getBodyBegin( void ) {

	return (this->raw_body_.begin());
}

std::string::iterator	Request::getBodyEnd( void ) {
	
	return (this->raw_body_.end());
}

std::vector<u_int8_t>::iterator	Request::getBodyVectorBegin( void ) {

	return (this->body_vector_.begin());
}

std::vector<u_int8_t>::iterator	Request::getBodyVectorEnd( void ) {
	
	return (this->body_vector_.end());
}

const std::string&	Request::getBody( void ) const {

	return this->raw_body_;
}
/************** PUBLIC SETTERS **************/

void	Request::setCgiFlag( bool flag) {
	
	this->cgi_flag_ = flag;
}

/************** CLASS PRIVATE METHODS **************/

/************** PRIVATE SETTERS **************/

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

void	Request::setChunked( void ) {

	std::string	transfer_encoding = this->headers_["Transfer-Encoding"];
	if (transfer_encoding == "chunked") {
		this->chunked_ = true;
	}
	else {
		this->chunked_ = false;
	}
}

void	Request::setKeepAlive( void ) {

	std::string connection = this->headers_["Connection"];
	if (connection == "keep-alive") {
		this->keep_alive_ = true;
	}
	else {
		this->keep_alive_ = false;
	}
}

void	Request::setCgiFlag( void ) {

	std::string	uri = this->getRequestLineValue("uri");
	if (strncmp(uri.c_str(), "/cgi-bin", 8) == 0) {
		this->cgi_flag_ = true;
	}
	else {
		this->cgi_flag_ = false;
	}
}

void	Request::setRequestAttributes( void ) {

	void	(Request::*setters[])(void) = { &Request::setKeepAlive, &Request::setChunked, &Request::setBodySize , &Request::setCgiFlag };
	for (int i = 0; i < 4; i++) { //get size of setters instead of 3
		(this->*setters[i])();
	}
}

void	Request::parseRequestLine_( std::string& to_parse ) {

	std::stringstream	ss(to_parse);
	std::string			part;

	if (ss.fail() || ss.bad()) {
		throw(std::runtime_error("Stringstream failed in parseRequestLine_"));
	}
	ss >> part;
	this->request_line_["method"] = part;
	ss >> part;
	this->request_line_["uri"] = part;
	ss >> part;
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
	// std::cout << "BODY PRINT IN SAVEBODY_ : \n" << this->raw_body_;
	// std::string::const_iterator body_it = this->raw_body_.begin();
	// for (; body_it != this->raw_body_.end(); body_it++) {
	// 	std::cout << *body_it;
	// }
	// std::cout << "BODY PRINT FINISHED\n";
}

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

void	Request::parseBody_( void ) {
	//check if body present with GET request
	//check body type (only plain for now)
	std::string content_type_header = getHeaderValueByKey("Content-Type");
	bool		is_multipart_form = content_type_header.find("multipart/form-data") != std::string::npos ? true : false;

	if (this->chunked_) {
		parseChunkedBody_();
		return ;
	}
	if (is_multipart_form) {
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

void	Request::parseChunkedBody_( void ) {

	//to be written
}

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

void	Request::setFilename( const std::string& to_parse ) {

	int fname_start = to_parse.find("filename=") + 9;

	std::string fname = to_parse.substr(fname_start);
	fname.erase(0,1);
	fname.erase(fname.length() - 3, 3);
	this->file_name_ = fname;

	this->file_upload_ = true;
}

void	Request::parseMultipartForm_( std::string boundary ) {

	std::string 	section_bound = "--" + boundary;
	std::string		last_bound = section_bound + "--\r\n"; section_bound.append("\r\n");
	std::string 	parse_buffer = "";
	size_t			body_index = 0;
	bool			file_present = false;

// this->raw_body_.find(section_bound, body_index!= std::string::npos)
	while (body_index < this->raw_body_.size()) {
		parse_buffer += this->raw_body_[body_index++];
		//line processed
		// std::cout << "Parse buffer before append to process body: " << parse_buffer << std::endl;
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
				this->processed_body_.append(parse_buffer);
			}
			parse_buffer.clear();
		}
		// ++body_index;
	}
		// if (!this->file_upload_ && parse_buffer.find("filename="))
		// 	this->file_upload_ == true;
		// if (this->file_upload_ && parse_buffer.find()){}
}
