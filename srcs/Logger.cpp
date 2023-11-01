
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

Logger::Logger( void ) {}

Logger::Logger( const Logger& to_copy ) {

	*this = to_copy;
} 

/* DESTRUCTOR */

Logger::~Logger( void ) {} 

/* OPERATOR OVERLOADS */

Logger&	Logger::operator=( const Logger& rhs ) {

	(void)rhs;
	return *this;
}

/* CLASS PUBLIC METHODS */
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

void	Logger::log( int msg_type, const char *msg_color, const char *msg, ... ) {

	std::string	timestamp = getTimestampString();

	va_list	args;
	va_start(args, msg);

	char	buffer[1024];
	vsprintf(buffer, msg, args);

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
	if (msg_type == E_DEBUG) {
		if (Logger::log_to_console_) {
			std::cout << msg_color << "[" << timestamp << "]\t[DEBUG]\t" << buffer << COLOR_RESET << std::endl;
		}
		if (Logger::log_to_files_) {
			Logger::all_log_file_ << "[" << timestamp << "]\t[DEBUG]\t" << buffer << std::endl;
			Logger::info_log_file_ << "[" << timestamp << "]\t[DEBUG]\t" << buffer << std::endl;
		}
	}
}

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
bool	Logger::checkIfToLogInFiles( void ) {

	struct stat info;

	if (stat(LOG_DIR, &info) == 0 && S_ISDIR(info.st_mode)) {
		Logger::log_dir_already_exists_ = true;

		std::string	input;
		std::cout << LOG_DIR << " -directory already exists; do you want to write over previous log files?" << std::endl
				<< "(will overwrite files " << LOG_ALL << ", " << LOG_ERROR << ", " << LOG_INFO << " and " << LOG_DEBUG << ")" << std::endl;
		while (true) {
			std::cout << "answer y/n: ";
			std::getline(std::cin, input);
			if (input == "n" || input == "y")
				break;
			std::cout << std::endl;
		}
		if (input == "n") {
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

bool	Logger::openLogFiles( void ) {
	
	bool opening_success = true;

	Logger::all_log_file_.open(LOG_DIR "/" LOG_ALL, std::ios::out | std::ios::trunc);
	Logger::error_log_file_.open(LOG_DIR "/" LOG_ERROR, std::ios::out | std::ios::trunc);
	Logger::info_log_file_.open(LOG_DIR "/" LOG_INFO, std::ios::out | std::ios::trunc);
	Logger::info_log_file_.open(LOG_DIR "/" LOG_DEBUG, std::ios::out | std::ios::trunc);
	if (Logger::all_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_ALL << "for writing." << std::endl;
		opening_success = false;
	}
	if (Logger::error_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_ERROR << "for writing." << std::endl;
		opening_success = false;
	}
	if (Logger::info_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_INFO << "for writing." << std::endl;
		opening_success = false;
	}
	if (Logger::debug_log_file_.fail()) {
		std::cerr << "Error opening " << LOG_DEBUG << "for writing." << std::endl;
		opening_success = false;
	}
	return opening_success;
}