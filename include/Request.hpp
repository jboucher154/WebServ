#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include "utility.hpp"
# include "Logger.hpp"
# include "MimeTypes.hpp"
# include "ResponseCodes.hpp"

/** \brief Carriage Return Line Feed
 * 
 * In the HTTP protocol, the CR-LF sequence is always used to terminate a line.
 */
# define CRLF "\r\n"

/*! \brief macro for request's time limit (in seconds)
 *
 * The request must be fully received in the set time limit, otherwise the
 * server will send an error code back to the client.
 */
# define REQUEST_TIMEOUT_SEC 10

class	Request {

	private:
		/* PRIVATE METHODS AND MEMBERS */
		size_t								body_size_;				/*!< \brief size_t that represents the request body size */
		size_t								body_len_received_;		/*!< \brief size_t that represents the amount of bytes of the body received */
		bool 								chunked_;				/*!< \brief bool that represents if the message is chunked or not */
		bool								keep_alive_;			/*!< \brief bool that represents if the connection will be kept alive or shut down after a response is sent*/
		bool								cgi_flag_;				/*!< \brief bool that represents if the request was for a cgi resource or not */
		bool								headers_complete;		/*!< \brief bool that represents if all the headers are received or not */
		bool								complete_;				/*!< \brief bool that represents if the request is fully received or not */
		std::string							host_name_;				/*!< \brief string of the hostname */
		int									port_;					/*!< \brief int of the port number */
		std::map<std::string, std::string>	request_line_;			/*!< \brief map of the request line's parts and their values */
		std::map<std::string, std::string>	headers_;				/*!< \brief map of the request's headers and their values */
		std::string							raw_body_;				/*!< \brief string of the raw body (vis-a-vis processed_body_)*/
		std::string							processed_body_;		/*!< \brief string of the processed body (vis-a-vis raw_body_)*/
		std::string							file_content_;			/*!< \brief string of a file's contents */
		std::string							file_name_;				/*!< \brief string of a file's name */
		std::vector<u_int8_t>				body_vector_;			/*!< \brief vector consisting of unsigned 8-bit integers of the body */
		bool								file_upload_;			/*!< \brief bool indicating if a file has been stored for upload */
		std::string							file_mime_;				/*!< \brief string of the mime type of a file */
		unsigned int						status_code_;			/*!< \brief unsigned int that represents the status code */
		time_t								request_start_time_;	/*!< \brief time_t that represents the time of when the all of the headers have been received */
		bool								query_encode_;			/*!< \brief bool that represents if there is a encoded query in the request or not */

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
		bool				getQueryEncode( void ) const;

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
		void	setCgiFlag( bool flag);
		void	clearUploadContent( void );
};

#endif
