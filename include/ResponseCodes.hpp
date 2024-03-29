#ifndef RESPONSECODES_HPP
# define RESPONSECODES_HPP

# include <map>
# include <string>

/*! \brief all of the status codes asigned to their discriptive strings.
*   
*/
typedef	enum	e_StatusCode {
	
	E_UNSET = 0,
	E_SERVER_PROCESSING = 1,

	// Information responses
	E_CONTINUE = 100,
	E_SWITCHING_PROTOCOLS = 101,
	E_PROCESSING = 102,
	E_EARLY_HINTS = 103,

	// Successful responses
	E_OK = 200,
	E_CREATED = 201,
	E_ACCEPTED = 202, 
	E_NON_AUTHORITATIVE_INFORMATION= 203, 
	E_NO_CONTENT = 204,
	E_RESET_CONTENT = 205,
	E_PARTIAL_CONTENT = 206,
	E_MULTI_STATUS = 207,
	E_ALREADY_REPORTED = 208,
	E_IM_USED = 226,

	// Redirection messages
	E_MULTIPLE_CHOICES = 300,
	E_MOVED_PERMANENTLY = 301,
	E_FOUND = 302,
	E_SEE_OTHER = 303,
	E_NOT_MODIFIED = 304,
	E_USE_PROXY = 305,
	E_UNUSED = 306,
	E_TEMPORARY_REDIRECT = 307,
	E_PERMANENT_REDIRECT = 308,

	// Client error responses
	E_BAD_REQUEST = 400,
	E_UNAUTHORIZED = 401, 
	E_PAYMENT_REQUIRED= 402, 
	E_FORBIDDEN = 403,
	E_NOT_FOUND = 404,
	E_METHOD_NOT_ALLOWED = 405,
	E_NOT_ACCEPTABLE = 406,
	E_PROXY_AUTHENTICATION_REQUIRED = 407, 
	E_REQUEST_TIMEOUT = 408,
	E_CONFLICT = 409,
	E_GONE = 410,
	E_LENGTH_REQUIRED = 411,
	E_PRECONDITION_FAILED = 412,
	E_PAYLOAD_TOO_LARGE = 413,
	E_URI_TOO_LONG = 414,
	E_UNSUPPORTED_MEDIA_TYPE= 415,
	E_RANGE_NOT_SATISFIABLE = 416,
	E_EXPECTATION_FAILED = 417,
	E_IM_A_TEAPOT = 418,
	E_MISDIRECTED_REQUEST= 421,
	E_UNPROCESSABLE_CONTENT = 422,
	E_LOCKED = 423,
	E_FAILED_DEPENDENCY = 424,
	E_TOO_EARLY = 425, 
	E_UPGRADE_REQUIRED = 426,
	E_PRECONDITION_REQUIRED = 428, 
	E_TOO_MANY_REQUESTS = 429, 
	E_REQUEST_HEADER_FEILDS_TOO_LARGE = 431,
	E_UNAVAILABLE_FOR_LEGAL_REAONSONS = 451,

	// Server error responses
	E_INTERNAL_SERVER_ERROR = 500,
	E_NOT_IMPLEMENTED = 501,
	E_BAD_GATEWAY = 502,
	E_SERVICE_UNAVAILABLLE = 503,
	E_GATEWAY_TIMEOUT = 504,
	E_HTTP_VERSION_NOT_SUPPORTED= 505,
	E_VARIANT_ALSO_NEGOTIATES= 506,
	E_INSUFFICIENT_STORAGE = 507,
	E_LOOP_DETECTED = 508,
	E_NOT_EXTENDED = 510,
	E_NETWORK_AUTHENTIFICATION_REQUIRED = 511
}				t_StatusCode;

/*! \brief This static class creates a map of Response Codes and creates
*         and prepares html responses out of them when appropriate methods are called.
*	
*	@class ResponseCodes
*/
class	ResponseCodes {

	private:
		ResponseCodes( void );
		ResponseCodes( const ResponseCodes& to_copy );
		~ResponseCodes( void );
		ResponseCodes&	operator=( const ResponseCodes& to_copy );

		static std::map<int, std::string>	codes_; /*!< \brief a static map of matching pairs of codes and the descriptive strings associated with them. */

		static void	initialize_codes( void );

	public:
		static std::string	getCodeElementBody( int code );
		static std::string	getCodeStatusLine( int code );
		static std::string	getCombinedStatusLineAndBody( int code );
};

#endif
