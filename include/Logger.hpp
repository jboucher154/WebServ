#ifndef LOGGER_HPP
# define LOGGER_HPP

// rm later
# include <iostream>
# include <fstream>
# include <sstream>

// keep here?
#include <sys/stat.h>
# include "WebServ.hpp"
# include "Color.hpp"
#include "utility.hpp"

# define LOG_BUFF_SIZE 1024


enum	e_where_to_log {
	E_LOG_ONLY_TO_CONSOLE,
	E_LOG_ONLY_TO_FILES,
	E_LOG_TO_BOTH_CONSOLE_AND_FILES
};

enum	e_log_msg_type {
	E_ERROR,
	E_INFO,
	E_DEBUG
};

# define LOG_LOCATION	E_LOG_ONLY_TO_CONSOLE
# define LOG_DIR		"webserv_log"
# define LOG_ALL		"log_all.txt"
# define LOG_ERROR		"log_error.txt"
# define LOG_INFO		"log_info.txt"
# define LOG_DEBUG		"log_debug.txt"


/*! \brief This static class logs the log messages in formats and files of the caller's choice. 
*  
*/
class	Logger {
	private:
		static bool			log_to_console_; /*!< @brief flag indicating if logs should be printed to console. */
		static bool			log_to_files_; /*!< @brief flag indicating if logs should be printed to file. */
		static bool			log_dir_already_exists_; /*!< @brief flag indicating if log file already exist or if they need to be created. */

		static std::ofstream	all_log_file_; /*!< @brief a log file ofsream that will be used to write all of the logs */
		static std::ofstream	error_log_file_; /*!< @brief a log file ofsream that will be used to write error logs */
		static std::ofstream	info_log_file_; /*!< @brief a log file ofsream that will be used to write info logs */
		static std::ofstream	debug_log_file_; /*!< @brief a log file ofsream that will be used to write debug logs */

		Logger( void );
		Logger( const Logger& to_copy );
		~Logger( void );
		Logger&	operator=( const Logger& to_copy );

		static bool	checkIfToLogInFiles( void );
		static void	initLogFiles( void );
		static bool	openLogFiles( void );

	public:

		static void	initLogger( void );
		static void	log( e_log_msg_type msg_type, const char *msg_color, const char *msg, ... );
		static void	closeLogFiles( void );

};

#endif
