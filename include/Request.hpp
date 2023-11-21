#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include "utility.hpp"
# include "Logger.hpp"

# define CRLF "\r\n" //make this project wide?

class	Request {

	private:
		/* PRIVATE METHODS AND MEMBERS */
		size_t								body_size_;
		size_t								body_len_received_;					
		bool 								chunked_;
		bool								keep_alive_;
		bool								cgi_flag_;
		bool								headers_complete;
		bool								complete_;
		bool								sever_error_;
		std::map<std::string, std::string>	request_line_;
		std::map<std::string, std::string>	headers_;
		std::vector<std::string>			text_body_;
		std::vector<char>					binary_body_;//maybe take out, not currenlty using
		//no footers for now

		void	parseRequestLine_( std::string& to_parse );
		void	parseHeader_( std::string& to_parse );
		void	parseBody_( std::string& to_parse );
		void 	storeBinaryBody_( std::string& to_parse);
		void	setBodySize( void );
		void	setChunked( void );
		void	setKeepAlive( void );
		void	setRequestAttributes( void );
		void	setCgiFlag( void );

	public:
		Request( void );
		Request( const Request& to_copy );

		~Request( void );

		Request&	operator=( const Request& to_copy );

		/* PUBLIC METHODS */
		void	add( std::string to_add );
		void	clear( void );

		/* GETTERS */
		size_t		getBodySize( void ) const;
		size_t		getBodyLengthReceived( void ) const;
		bool		getChunked( void ) const;
		bool		getKeepAlive( void ) const;
		bool		getComplete( void ) const;
		bool		getServerError( void ) const;
		bool		getCgiFlag( void ) const;
		std::vector<char>::iterator	getBinaryBodyBegin( void );
		std::vector<char>::iterator	getBinaryBodyEnd( void );

		void		printRequest( void ) const;

		std::string											getRequestLineValue( std::string key ) const;
		std::map<std::string, std::string>::const_iterator	getHeaderBegin( void ) const;
		std::map<std::string, std::string>::const_iterator	getHeaderEnd( void ) const;
		std::string											getHeaderValueByKey( std::string key ) const;
		std::vector<std::string>::iterator	getBodyBegin( void );
		std::vector<std::string>::iterator	getBodyEnd( void );
		// const std::string&	getBody() const;
};

#endif

/* Request */
// Get /mysite/index.html HTTP/1.1\r\n
// Host: 10.101.101.10\r\n
// Accept: */*\r\n
// \r\n

/* Response */
// HTTP/1.1 200 OK\r\n
// Content-Length: 55\r\n
// Content-Type: text/html\r\n
// Last-Modified: Wed, 12 Aug 1998 15:03:50 GMT\r\n
// Accept-Ranges: bytes\r\n
// ETag: “04f97692cbd1:377”\r\n
// Date: Thu, 19 Jun 2008 19:29:07 GMT\r\n
// \r\n
// <55-character response>