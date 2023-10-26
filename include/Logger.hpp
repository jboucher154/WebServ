#ifndef LOGGER_HPP
# define LOGGER_HPP

// rm later
#include <iostream>
#include <fstream>
#include <sstream>

// keep here?
#include <sys/stat.h>

enum	e_log_msg_type {
	E_ERROR,
	E_INFO
};

enum	e_where_to_log {
	E_LOG_ONLY_TO_CONSOLE,
	E_LOG_ONLY_TO_FILES,
	E_LOG_TO_BOTH_CONSOLE_AND_FILES
};

# define LOG_LOCATION	E_LOG_TO_BOTH_CONSOLE_AND_FILES
# define LOG_DIR		"webserv_log"
# define LOG_ALL		"all_log.txt"
# define LOG_ERROR		"error_log.txt"
# define LOG_INFO		"info_log.txt"

/*! \brief Brief description.
*         Brief description continued.
*
*  Detailed description starts here.
*/

class	Logger {

	private:
		static bool			log_to_console_;
		static bool			log_to_files_;
		static bool			log_dir_already_exists_;

		static std::ofstream	all_log_file_;
		static std::ofstream	error_log_file_;
		static std::ofstream	info_log_file_;

		Logger( void );
		Logger( const Logger& to_copy );
		~Logger( void );
		Logger&	operator=( const Logger& to_copy );

		static bool	checkIfToLogInFiles( void );
		static void	initLogFiles( void );
		static bool	openLogFiles( void );

	public:
		static void	initLogger( void );
		static void	log( int msg_type, const char *msg_color, const char *msg, ... );
		static void	closeLogFiles( void );

};

#endif
