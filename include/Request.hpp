#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <map>

# define CRLF "\r\n" //make this project wide?

class	Request {

	private:
		/* PRIVATE METHODS AND MEMBERS */
		// things I think will be good to have a s a quick reference for the request handling
		int									size_;
		bool 								chunked_;
		bool								keep_alive_;
		bool								headers_complete;
		std::map<std::string, std::string>	request_line_; // method, uri, version
		std::map<std::string, std::string>	headers_; //host, content-type, user-agent, content-egth, last-modified, PRAGMA, accept-encoding accept, connection, reffer, etc.
		std::map<std::string, std::string>	body_; //body of the request, should it be a map?
		//no footers for now

		void	parseRequestLine_( std::string to_parse );
		void	parseHeader_( std::string to_parse );
		void	parseBody_( std::string to_parse );



	public:
		Request( void ); // itialize to default values
		// Request( /*add parameters*/ ); // no paramertized constructor for now
		Request( const Request& to_copy );

		~Request( void );

		Request&	operator=( const Request& to_copy );

		/* PUBLIC METHODS */
		void	add( std::string to_add ); //add to any part of the response, will direct to correct private parsing function
		void	clear( void ); //clear all data in the request

		/* GETTERS */
		int		getSize( void ) const;
		bool	getChunked( void ) const;
		bool	getKeepAlive( void ) const;
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