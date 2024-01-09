#include "Logger.hpp"
#include "ServerManager.hpp"
#include "Validator.hpp"

/*!	\brief main entry to program
*
*	Intializes logger and calls to validate config file. If no errors encountered in the server
*	setup, the servermanager will be called to run the main loop server loop.	
*
*	
*	@authors Jenny Boucher, Azär Sarikhani Aghmiouni, Sakari Salmi
*	@param argc	count of command line arguments
*	@param argv C-style character array of command line arguments
*	@return @b int description of the return value	returns EXIT_FAILURE in case of error encountered, otherwise EXIT_SUCCESS
*/
int	main( int argc, char *argv[]) {

	Logger::initLogger();
	if (argc > 2) {
		Logger::log(E_INFO, COLOR_GREEN, "Usage: ./webserv config.conf or ./webserv");
		Logger::closeLogFiles();
		return EXIT_FAILURE;
	}
	else if (argc == 2 && !Validator::validate(argv[1])) {
		Logger::closeLogFiles();
		return EXIT_FAILURE;
	}
	else if (argc == 1){
		if (!Validator::validate(DEFAULT_CONFIG_PATH)){
	    	Logger::log(E_INFO, COLOR_RED, "Default config is not valid!");
	    	Logger::closeLogFiles();
	    	return EXIT_FAILURE;
		}
	}
	for (size_t i = 0; i < Validator::servers.size(); i++){
		if (!Validator::servers[i].setUploadStore(".temp_files"))
			return EXIT_FAILURE;
	}
	#if POLL_TRUE_SELECT_FALSE
		// POLL VERSION
		while (true) {
			ServerManager server_manager(Validator::servers);
			if (!server_manager.POLL_initializeServers()) {  
				server_manager.closeServerSockets();
				Logger::closeLogFiles();
				return EXIT_FAILURE;
			}
			if (server_manager.POLL_runServers())
				break;
			Logger::log(E_INFO, COLOR_BRIGHT_CYAN, "Servers stopped running because of an poll error, RESTARTING SERVERS!!!");
		}
	#else
	//SELECT VERSION
		while (true) {
			ServerManager server_manager(Validator::servers);
			if (!server_manager.SELECT_initializeServers()) {  
				server_manager.closeServerSockets();
				Logger::closeLogFiles();
				return EXIT_FAILURE;
			}
			if (server_manager.SELECT_runServers())
				break;
			Logger::log(E_INFO, COLOR_BRIGHT_CYAN, "Servers stopped running because of an select error, RESTARTING SERVERS!!!");
		}
	#endif
	Logger::closeLogFiles();
	return EXIT_SUCCESS;
}
