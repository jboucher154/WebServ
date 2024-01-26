#ifndef	WEBSERV_HPP
/*! \brief this header file contains important macros for the server,
*           varioussettings can be edited from here.
*
*   This header file contains important macros for the server various server 
*   settings can be edited from here.
*   Especially server timout settings for clients, cgi, response, shutdown timeout
*   The default path for the config can be set here as well.
*
*   
*/
# define WEBSERV_HPP

/*! \brief Macro for selecting POLL or SELECT Version of Program
*
*   Marco for selecting POLL or SELECT Version of Program. 
*   true = compile with POLL veresion
*   false = compile with SELECT version
*   
*/
# ifndef POLL_TRUE_SELECT_FALSE
#  define POLL_TRUE_SELECT_FALSE true
# endif

/*! \brief Macro for logging debug statements
*
*   Macro for logging debug statements. The output will follow the Logger settings,
*   (to file, terminal, or both).
*   true = print debug information
*   false = no debug output logged
*/
# ifndef GET_DEBUG_LOG
#  define GET_DEBUG_LOG	false
# endif

/*! \brief Macro for enabling/disabling the getting of fd info for main server loop
*   
*   Macro for enabling/disabling the getting of fd info for main server loop. 
*   CAUTION: this can cause a flood the terminal/logs, but useful in some cases
*   true = print the fd info
*   false = no printing of fd information
*   
*/
# if GET_DEBUG_LOG
#  ifndef GET_SELECT_POLL_LOOP_FD_INFO
#   define GET_SELECT_POLL_LOOP_FD_INFO false
#  endif
# endif

/*! \brief Macro for the default config file
*
*   Macro for the default config file. If no arguments given when `./webserv' is
*   launched this path will be used for a default config.
*   path should be relative to the root where the server is launched.
*
*/
# ifndef DEFAULT_CONFIG_PATH
#  define DEFAULT_CONFIG_PATH "config/good/default.conf"
# endif


/*********************************************** ServerManager macros ***********************************************/

/*! \brief sets timeout for select() in seconds
*
*   sets timeout for select() in seconds.
*
*/
# define SELECT_TIMEOUT_SEC 2

/*! \brief sets timeout for select() in microseconds
*
*   sets timeout for select() in microseconds.
*
*/
# define SELECT_TIMEOUT_USEC 0

/*! \brief sets timeout for poll() in milliseconds
*
*   sets timeout for poll() in milliseconds.
*
*/
# define POLL_TIMEOUT_MILLISEC 2 * 1000

/*! \brief sets time the servers will run without any client action (in seconds)
*
*   Time the servers will run for without any client action (in seconds).
*
*/
# define SERVER_SHUTDOWN_TIME_SEC 1000 * 60

/*! \brief time a client connection is kept open from last client action (in seconds)
*
*   Time a client connection is kept open from last client action (in seconds).
*   Client connection will be terminated if this timeout is reached.
*
*  
*/
# define CLIENT_TIMEOUT_SEC 1 * 10

/*! \brief bytes size of buffer set for client with setsockopt()
*
*	After accepting the client the buffer will be set to this using the SO_SNDBUF option.
*	This should be checked with each new system and adjusted to meet system limitations.
*/
# define CLIENT_BUFFER_SIZE 4409640

/*********************************************** CGI macros ***********************************************/

/*! \brief sets timeout for cgi script processing
*
*   Sets timeout for cgi script processing. If the timeout is reached a 
*   504 Gateway Timeout response will be sent.
*
*/
# define CGI_TIMEOUT 5

/*! \brief sets the max size for cgi output reading
*
*   Sets the max size size for cgi output reading. This will vary based on system limitations
*   with pipes.
*
*/
# define CGI_OUTPUT_BUFFER 64000

#endif

/*********************************************** Request macros ***********************************************/


/*********************************************** Response macros ***********************************************/

