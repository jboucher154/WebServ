// rm later; testing
#include "Logger.hpp"
#include "Color.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

#include <iostream>
#include <vector>

#include <string>

int  main( int argc, char *argv[]) {

  Logger::initLogger();
  (void)argv;
  if (argc == 2) {
    std::cout << "parse will be called!" << std::endl;
  }
  std::vector<Server> server_vector;

  Server  server1;
  server1.setHost("127.0.0.1");
  server1.setListeningPort(8080);
  server1.setServerName("Server-1");
  server_vector.push_back(server1);
  std::vector<std::string> values;
  values.push_back("GET");
  values.push_back("HEAD"); 
  server1.setLocation( "/blue", "allow_methods", values );
  Server  server2;
  server2.setHost("127.0.0.1");
  server2.setListeningPort(9090);
  server2.setServerName("Server-2");
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
  //   Logger::log(E_INFO, COLOR_BRIGHT_CYAN, "Servers stopped running because of an error, RESTARTING SERVERS!!!");
  // }

  Logger::closeLogFiles();

  return 0;
}
