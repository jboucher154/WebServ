#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <ctime>
# include <time.h>
# include <ostream>
# include <map>
# include <unistd.h>
# include <cstdio>
# include <fstream>
# include <vector>

# include "ResponseCodes.hpp"
# include "utility.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "Logger.hpp"
# include "MimeTypes.hpp"

/*! \brief File size limit for pipes
 * 
 */
# define FILE_SIZE_LIMIT_FOR_PIPE 64000

/*! \brief Class for handling HTTP responses.
*       
*	@class Response
*
* 	Response class that handles the server's HTTP responses to its clients.
*	It is responsible for generating the response to a client request.
*  
*  Public Methods:
*  - generate: generate the response given a Request object
*  - send: send the response to the client
*  - clear: reset the response for next use
*/
class	Response {

	private:
		Response( void );

		/* PRIVATE METHODS AND MEMBERS */
		std::string			response_;			/*!< \brief string of the entire response */
		std::string			body_;				/*!< \brief string of the body of the response */
		std::string			response_mime_;		/*!< \brief string of the response's body's mime type */
		std::string			resource_path_; 	/*!< \brief string of the path of the resource requested in the uri */
		std::string			alias_location_;	/*!< \brief string of the actual location value a alias key points to */
		std::string			resource_location_; /*!< \brief string of the name of the resource location in the uri */
		int					status_code_;		/*!< \brief status code of the response */
		Server*				server_;			/*!< \brief pointer to the server the response gets its resources from */
		Request*			request_;			/*!< \brief pointer to request this response is tied to */
		bool				redirect_;			/*!< \brief bool that represents if there is a redirection present in the location or not */
		bool				alias_;				/*!< \brief bool that tells if there is an alias in the resource location */
		bool				directory_listing_;	/*!< \brief bool that represents if there is autoindex is true in the requested location or not */
		std::string			query_string_;		/*!< \brief string of the query string passed as a metavariable for a cgi-script */
		std::string			temp_filepath_;		/*!< \brief string of the temporary filepath that may be included in the response */
		bool				temp_file_;			/*!< \brief bool that represents if there is a temporary file involved in the response or not */

		/* HEADER GENERATORS */
		std::string&	addHeaders_( std::string& response) const;
		std::string		timeStampHeader_( void ) const;
		std::string		contentTypeHeader_( void ) const;
		std::string 	contentLengthHeader_( void ) const;
		std::string		contentLocationHeader_( void ) const;
		std::string		locationHeader_( void ) const;
		std::string		retryAfterHeader_( void ) const;
		std::string		connectionHeader_( bool connection_continue ) const;

		/* METHODS */
		void	getMethod_( void );
		void	headMethod_( void );
		void	deleteMethod_( void );
		void	postMethod_( void );

		bool	methodAllowed_( std::string method );

		/* UTILITIES FOR GET */
		void	buildBody_( std::string& path, std::ios_base::openmode mode );
		std::vector<std::string>	getAcceptedFormats_( void );

		/* UTILITIES FOR POST */
		void	saveBodyToFile_( bool is_save_dir );
		void	saveBodyToTempFile_( void );
		void	deleteTempFile_( void );
		
		/* RESOURCE AND LOCATION IDENTIFICATION */
		int		setResourceLocationAndName_( std::string uri );
		void	setResourceLocation_( std::string& uri, bool is_dir, size_t last_slash_pos );
		void	setResourcePath_( std::string& uri, bool is_dir, size_t last_slash_pos );
		bool	handleRedirection_( void );
		void	handelAlias_( void );

		void	setMimeType_( void );
		void	validateResource_( void );
		void	createErrorBody_( void );


		/*TYPEDEF*/
		typedef	void	(Response::*response_methods_[]) ( void );

		/* DIRECTORY LISTING*/
		void 		listFiles(const std::string& path, std::vector<std::string>& directoryVec);
		std::string buildHtmlList(const std::string& path);

	public:
		Response( const Response& to_copy );
		Response( Server* server );
		~Response( void );

		Response&	operator=( const Response& to_copy );

		/* PUBLIC METHODS */

		void			createResponsePhase1( Request* request );
		void			clear( void );
		std::string&	buildAndGetResponsePhase2( void );
		std::string&	buildAndGetResponsePhase2( const std::string& body );

		/* GETTERS */
		int						getStatusCode( void ) const;
		const std::string&		getResourcePath( void ) const;
		const std::string&		getQueryString( void ) const;
		const std::string&		getUploadData( void );

		/* SETTERS */
		void			setStatusCode( unsigned int	new_code );
		void			setServer( Server* server );
};

#endif
