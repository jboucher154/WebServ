// rm later; testing
#include "Logger.hpp"
#include "Color.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Validator.hpp"
#include <iostream>
#include <vector>
#include <string>

int	main( int argc, char *argv[]) {

	Logger::initLogger();
	(void)argv;
	if (argc > 2) {
		Logger::log(E_INFO, COLOR_GREEN, "Usage: ./Webserver config.conf or ./Webserver");
		Logger::closeLogFiles();
		return 1;
	}
	else if (argc == 2 && !Validator::validate(argv[1])) {
		Logger::closeLogFiles();
		return 1;
	}
	else if (argc == 1){
		if (!Validator::validate("config/good/default.conf")){
	    	Logger::log(E_INFO, COLOR_RED, "Default config is not valid!");
	    	Logger::closeLogFiles();
	    	return 1;
		}
	}
	for (size_t i = 0; i < Validator::servers.size(); i++){
		if (!Validator::servers[i].setUploadStore(".temp_files"))
			return 1;
	}
	#if POLL_TRUE_SELECT_FALSE
		// POLL VERSION
		while (true) {
			ServerManager server_manager(Validator::servers);
			if (!server_manager.POLL_initializeServers()) {  
				server_manager.closeServerSockets();
				Logger::closeLogFiles();
				return 1;
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
				return 1;
			}
			if (server_manager.SELECT_runServers())
				break;
			Logger::log(E_INFO, COLOR_BRIGHT_CYAN, "Servers stopped running because of an select error, RESTARTING SERVERS!!!");
		}
	#endif
	Logger::closeLogFiles();
	return 0;
}
