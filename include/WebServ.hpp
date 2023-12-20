/*  
    THIS HPP FILE IS FOR ALMOST ALL MACROS IN THE PROJECT
    THIS IS VERY USEFUL: IF YOU WANT TO CHANGE SOMETHING, YOU CAN JUST EDIT THIS FILE!

    THE FIRST PART ARE BOOL MACROS WHERE YOU CAN ENABLED CERTAIN FEATURES OR NOT
    THE SECOND PART CONTAINS TIME SETTINGS FOR THE SERVERMANAGER
    THE THIRD PART CONTAINS TIME SETTINGS FOR CGI
*/

#ifndef	WEBSERV_HPP
# define WEBSERV_HPP

// MACRO FOR SELECTING POLL OR SELECT VERSION OF PROGRAM
# ifndef POLL_TRUE_SELECT_FALSE
#  define POLL_TRUE_SELECT_FALSE true
# endif

// MACRO FOR GETTING MORE DEBUG LOG (MORE INFO)
# ifndef GET_DEBUG_LOG
#  define GET_DEBUG_LOG	true
# endif

// NEW MACRO FOR ENABLING/DISABLING GETTING FD INFO FOR SELECT/POLL LOOP
// (they flood the terminal/logs, but useful in some cases)
# if GET_DEBUG_LOG
#  ifndef GET_SELECT_POLL_LOOP_FD_INFO
#   define GET_SELECT_POLL_LOOP_FD_INFO false
#  endif
# endif

// MACRO FOR SWITCHING BETWEEN HIVE COMPUTERS AND MACBOOKS (REMOVE LATER?)
// (switch between desktop version (default.conf and logger uses vsprintf) and macbook version (default_macbook.conf and logger uses vsnprintf))
# ifndef HIVE_DESKTOP_OR_MACBOOK
#  define HIVE_DESKTOP_OR_MACBOOK false
# endif


/*********************************************** ServerManager macros ***********************************************/

// for timing out select()
# define SELECT_TIMEOUT_SEC 2
# define SELECT_TIMEOUT_USEC 0

// for timing out poll()
# define POLL_TIMEOUT_MILLISEC 2 * 1000

// time the servers will run for without any client action (in seconds)
# define SERVER_SHUTDOWN_TIME_SEC 5 * 60

// time a client connection is kept open from last client action (in seconds)
# define CLIENT_TIMEOUT_SEC 1 * 10


/*********************************************** CGI macros ***********************************************/

// macro for cgi timeout
# define CGI_TIMEOUT 5

// macro for buffer for cgi output reading
# define CGI_OUTPUT_BUFFER 102400

#endif