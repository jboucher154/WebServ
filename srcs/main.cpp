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
    #ifdef HIVE_DESKTOP_OR_MACBOOK
    if (!Validator::validate("config/good/default.conf")){
        Logger::log(E_INFO, COLOR_RED, "Default config is not valid!");
        Logger::closeLogFiles();
        return 1;
      }
    #else
      if (!Validator::validate("config/good/default_macbook.conf")){
        Logger::log(E_INFO, COLOR_RED, "Default (Macbook) config is not valid!");
        Logger::closeLogFiles();
        return 1;
      }
    #endif

  }
  std::cout << " server 1 index: " << Validator::servers[0].getIndex() << std::endl; 
  std::cout << " server 1 index: " << Validator::servers[1].getIndex() << std::endl;
  std::cout << " servers size: " << Validator::servers.size() << std::endl;
  std::cout << " server 1 index: " << (Validator::servers[0].isKeyInLocation("/cgi-bin", "root") ? "true" : "false") << std::endl;
  std::vector<Server> server_vector;

  Server  server1("Server-1", 8080, "127.0.0.1", "./resources", "index,html", "2147483647");
  server_vector.push_back(server1);
  std::cout << (server1.isKeyInLocation("/blue", "allow_methods") ? "true" : "false") << std::endl;
  std::cout << (server_vector.front().isKeyInLocation("/blue", "allow_methods") ? "true" : "false") << std::endl;
  Server  server2("Server-2", 9090, "127.0.0.1", "./resources", "index,html", "2147483647");
  server_vector.push_back(server2);
  
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
  #endif


  Logger::closeLogFiles();

  return 0;
}
