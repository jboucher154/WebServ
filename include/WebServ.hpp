#ifndef	WEBSERV_HPP
# define WEBSERV_HPP

// MACRO FOR SELECTING POLL OR SELECT VERSION OF PROGRAM
# ifndef POLL_TRUE_SELECT_FALSE
#  define POLL_TRUE_SELECT_FALSE true
# endif

// MACRO FOR GETTING MORE DEBUG LOG (MORE INFO)
# ifndef GET_DEBUG_LOG
#  define GET_DEBUG_LOG	false
# endif

// MACRO FOR SWITCHING BETWEEN HIVE COMPUTERS AND MACBOOKS (REMOVE LATER?)
// (switch between desktop version (default.conf and logger uses vsprintf) and macbook version (default_macbook.conf and logger uses vsnprintf))
# ifndef HIVE_DESKTOP_OR_MACBOOK
#  define HIVE_DESKTOP_OR_MACBOOK true
# endif

#endif