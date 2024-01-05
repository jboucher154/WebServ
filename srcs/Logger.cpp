
#include "Logger.hpp"
#include "TimeUtils.hpp"
#include "Color.hpp"

bool	Logger::log_to_console_ = true;
bool	Logger::log_to_files_ = true;
bool	Logger::log_dir_already_exists_ = false;
std::ofstream	Logger::all_log_file_;
std::ofstream	Logger::error_log_file_;
std::ofstream	Logger::info_log_file_;
std::ofstream	Logger::debug_log_file_;

/*! \brief Default constructor, not used for static class
*       
*  	Default constructor, not used for static class
*
*/
Logger::Logger( void ) {}

/*! \brief Copy constructor, not used for static class
*       
*  	Copy constructor, not used for static class
*
*/
Logger::Logger( const Logger& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

/*! \brief Defalut destructor, not used for static class
*       
*  	Defalut destructor, not used for static class
*
*/
Logger::~Logger( void ) {} 

/* OPERATOR OVERLOADS */

/*! \brief Assignment operator overload , not used for static class
*       
*  	Assignment operator overload , not used for static class
*
*/
Logger&	Logger::operator=( const Logger& rhs ) {

	(void)rhs;
	return *this;
}

/* CLASS PUBLIC METHODS */

/*! \brief initializes the log to console and log to files varaibles based on 
*				macros set at compile time.
*       
*  	Initializes the log to console and log to files varaibles based on 
*	macros set at compile time. If logging to files is set true, the log files
*	will be intialized.
*
*/
void	Logger::initLogger( void ) {

	if (LOG_LOCATION == E_LOG_ONLY_TO_FILES)
		Logger::log_to_console_ = false;
	else if (LOG_LOCATION == E_LOG_ONLY_TO_CONSOLE)
		Logger::log_to_files_ = false;
	if (Logger::log_to_files_) {
		if (Logger::checkIfToLogInFiles())
			initLogFiles();
	}
}

/*! \brief will write log message to designated logfile based on params
*       
*  	Will write log message to designated logfile (E_ERROR, E_INFO, E_DEBUG) 
*	indicated by msg_type in the color given by msg_color. The message will be
*	written to the std_out or std_err  and the appropriate logfile if file
*	logging is enabled.
*
*/
void	Logger::log( e_log_msg_type msg_type, const char *msg_color, const char *msg, ... ) {

	std::string	timestamp = getTimestampString();

	va_list	args;
	va_start(args, msg);

	char	buffer[LOG_BUFF_SIZE];

	vsnprintf(buffer, LOG_BUFF_SIZE, msg, args);

	va_end(args);

	if (msg_type == E_ERROR) {
		if (Logger::log_to_console_) {
			std::cerr << msg_color << "[" << timestamp << "]\t[ERROR]\t" << buffer << COLOR_RESET << std::endl;
		}
		if (Logger::log_to_files_) {
			Logger::all_log_file_ << "[" << timestamp << "]\t[ERROR]\t" << buffer << std::endl;
			Logger::error_log_file_ << "[" << timestamp << "]\t[ERROR]\t" << buffer << std::endl;
		}
	}
	if (msg_type == E_INFO) {
		if (Logger::log_to_console_) {
			std::cout << msg_color << "[" << timestamp << "]\t[INFO]\t" << buffer << COLOR_RESET << std::endl;
		}
		if (Logger::log_to_files_) {
			Logger::all_log_file_ << "[" << timestamp << "]\t[INFO]\t" << buffer << std::endl;
			Logger::info_log_file_ << "[" << timestamp << "]\t[INFO]\t" << buffer << std::endl;
		}
	}
	if (msg_type == E_DEBUG && GET_DEBUG_LOG == true) {
		if (Logger::log_to_console_) {
			std::cout << msg_color << "[" << timestamp << "]\t[DEBUG]\t" << buffer << COLOR_RESET << std::endl;
		}
		if (Logger::log_to_files_) {
			Logger::all_log_file_ << "[" << timestamp << "]\t[DEBUG]\t" << buffer << std::endl;
			Logger::debug_log_file_ << "[" << timestamp << "]\t[DEBUG]\t" << buffer << std::endl;
		}
	}
}

/*! \brief closes all logfiles if they are open
*       
*  	Closes all logfiles if they are open.
*
*/
void	Logger::closeLogFiles( void ) {

	if (Logger::all_log_file_.is_open()) {
		Logger::all_log_file_.close();
	}
	if (Logger::error_log_file_.is_open()) {
		Logger::error_log_file_.close();
	}
	if (Logger::info_log_file_.is_open()) {
        Logger::info_log_file_.close();
    }
	if (Logger::debug_log_file_.is_open()) {
        Logger::debug_log_file_.close();
    }
}

/* CLASS PRIVATE METHODS */

/*! \brief checks if directory for logfiles exists already and gives option to overwrite if it does exist.
*       
*  	Checks if directory for logfiles exists already and gives option to overwrite if it does exist.
*	if overwrite is not requested, not logging to files will occur.
*
*/
bool	Logger::checkIfToLogInFiles( void ) {

	struct stat info;

	if (stat(LOG_DIR, &info) == 0 && S_ISDIR(info.st_mode)) {
		Logger::log_dir_already_exists_ = true;

		std::string	input;
		std::cout << LOG_DIR << " -directory already exists; do you want to write over previous log files in the directory?" << std::endl
				<< "(will overwrite log_*.txt files)" << std::endl;
		while (true) {
			std::cout << "answer y/n: ";
			std::getline(std::cin, input);
			if (std::cin.eof() || input == "n" || input == "y")
				break;
			std::cout << std::endl;
		}
		if (std::cin.eof() || input == "n") {
			std::cout << "Webserv will not save log data into files!" << std::endl;
			if (LOG_LOCATION == E_LOG_ONLY_TO_FILES) {
				std::cout << "Will print log to console instead!" << std::endl;
				Logger::log_to_console_ = true;
			}
			return false;
		} else {
			return true;
		}
	}
	return true;
}

/*! \brief creates logfile directory if needed and calls to open the logfiles.
*       
*  	Creates logfile directory if needed and calls to open the logfiles. If error occurs
*	logging will be to console only.
*
*/
void	Logger::initLogFiles( void ) {

	if (!Logger::log_dir_already_exists_) {
		if (mkdir(LOG_DIR, 0777) == -1) {
			std::cerr << "Error creating log directory: " << strerror(errno) << std::endl
					<< "Will print log to console instead!" << std::endl;
			Logger::log_to_console_ = true;
			Logger::log_to_files_ = false;
			return;
		}
	}
	if (!Logger::openLogFiles()) {
		 std::cerr << "Error encountered when trying to open log files for writing." << std::endl
		 		<< "Will print log to console instead!" << std::endl;
		Logger::closeLogFiles();
		Logger::log_to_console_ = true;
		Logger::log_to_files_ = false;
	}
	return;
}

/*! \brief opens three logfiles, all_log.txt, log_error.txt, and log_info.txt
*       
*  	Opens three logfiles, all_log.txt, log_error.txt, and log_info.txt in the logfile 
*	directory configured. Will return false if any of the openings fail.
*
*/
bool	Logger::openLogFiles( void ) {
	
	bool opening_success = true;

	Logger::all_log_file_.open(LOG_DIR "/" LOG_ALL, std::ios::out | std::ios::trunc);
	Logger::error_log_file_.open(LOG_DIR "/" LOG_ERROR, std::ios::out | std::ios::trunc);
	Logger::info_log_file_.open(LOG_DIR "/" LOG_INFO, std::ios::out | std::ios::trunc);

	if (GET_DEBUG_LOG == true) {
		Logger::debug_log_file_.open(LOG_DIR "/" LOG_DEBUG, std::ios::out | std::ios::trunc);
		if (Logger::debug_log_file_.fail()) {
			std::cerr << "Error opening " << LOG_DEBUG << " for writing." << std::endl;
			opening_success = false;
		}
	}
	if (Logger::all_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_ALL << " for writing." << std::endl;
		opening_success = false;
	}
	if (Logger::error_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_ERROR << " for writing." << std::endl;
		opening_success = false;
	}
	if (Logger::info_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_INFO << " for writing." << std::endl;
		opening_success = false;
	}
	return opening_success;
}
