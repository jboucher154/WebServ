#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include "utility.hpp"
# include "Logger.hpp"
# include "MimeTypes.hpp"

# define CRLF "\r\n" //make this project wide?
# define REQUEST_TIMEOUT_SEC 10

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
		std::string							host_name_;
		int									port_;
		std::map<std::string, std::string>	request_line_;
		std::map<std::string, std::string>	headers_;
		std::string							raw_body_;
		std::string							processed_body_;
		std::string							file_content_; //could make vector for multiple....
		std::string							file_name_;
		std::vector<u_int8_t>				body_vector_;
		bool								file_upload_;//to tell if there is file data to save
		std::string							file_mime_;
		unsigned int						status_code_;
		time_t								request_start_time_;
		//no footers for now

		void	parseRequestLine_( std::string& to_parse );
		void	parseHeader_( std::string& to_parse );

		void 	saveBody_(std::string& to_add, size_t body_start, size_t total_bytes);
		void	parseBody_( void );
		void	parseChunkedBody_( void );
		void 	storeFileContents_( const std::string& section_bound, const std::string& last_bound, size_t& body_index );
		void	parseMultipartForm_( std::string boundary );
		void	setFilename( const std::string& to_parse );

		void	setBodySize( void );
		void	setChunked( void );
		void	setKeepAlive( void );
		void	setRequestAttributes( void );
		void	setCgiFlag( void );
		void	setHostNameAndPort( void );

	public:
		Request( void );
		Request( const Request& to_copy );

		~Request( void );

		Request&	operator=( const Request& to_copy );

		/* PUBLIC METHODS */
		void	add( char* to_add, size_t bytes_read );
		void	clear( void );
		void	printRequest( void ) const;

		/* GETTERS */
		size_t				getBodySize( void ) const;
		size_t				getBodyLengthReceived( void ) const;
		bool				getChunked( void ) const;
		bool				getKeepAlive( void ) const;
		bool				getComplete( void ) const;
		bool				getServerError( void ) const;
		bool				getCgiFlag( void ) const;
		const std::string&	getRequestHostName( void ) const;
		int					getRequestPort( void ) const;

		std::string											getRequestLineValue( std::string key ) const;
		std::map<std::string, std::string>::const_iterator	getHeaderBegin( void ) const;
		std::map<std::string, std::string>::const_iterator	getHeaderEnd( void ) const;
		std::string											getHeaderValueByKey( std::string key ) const;
		const std::string&		getProcessedBody( void ) const;
		const std::string&		getUploadContent( void ) const;
		const std::string&		getUploadName( void ) const;
		bool					isFileUpload( void ) const;
		unsigned int			getStatusCode( void ) const;
		const std::string&		getUploadMime( void ) const;
		bool					checkRequestTimeout( void ) const;

		/* SETTERS */
		void	setCgiFlag( bool flag);//public overload
};

#endif
