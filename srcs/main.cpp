// rm later; testing
#include "Logger.hpp"
#include "Color.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Validator.hpp"
#include <iostream>
#include <vector>

#include <string>

int  main( int argc, char *argv[]) {

  Logger::initLogger();
  (void)argv;
  if (argc == 2) {
    //std::cout << "parse will be called!" << std::endl;
   Validator::validate(argv[1]);
  }
  std::vector<Server> server_vector;

  Server  server1("Server-1", 8080, "127.0.0.1");
  server_vector.push_back(server1);
  Server  server2("Server-2", 9090, "127.0.0.1");
  server_vector.push_back(server2);
  //SELECT VERSION
  while (true) {
    ServerManager server_manager(server_vector);
    if (!server_manager.SELECT_initializeServers()) {  
      server_manager.closeServerSockets();
      Logger::closeLogFiles();
      return 1;
    }
    if (server_manager.SELECT_runServers())
      break;
    Logger::log(E_INFO, COLOR_BRIGHT_CYAN, "Servers stopped running because of an select error, RESTARTING SERVERS!!!");
  }

  // POLL VERSION
  //  while (true) {
  //   ServerManager server_manager(server_vector);
  //   if (!server_manager.POLL_initializeServers()) {  
  //     server_manager.closeServerSockets();
  //     Logger::closeLogFiles();
  //     return 1;
  //   }
  //   if (server_manager.POLL_runServers())
  //     break;
  //   Logger::log(E_INFO, COLOR_BRIGHT_CYAN, "Servers stopped running because of an poll error, RESTARTING SERVERS!!!");
  // }

  Logger::closeLogFiles();

  return 0;
}
