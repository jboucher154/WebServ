// rm later; testing
#include "Logger.hpp"
#include "Color.hpp"
// #include "Sockets.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

#include <iostream>
#include <vector>

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

  Server  server2;
  server2.setHost("127.0.0.1");
  server2.setListeningPort(9090);
  server2.setServerName("Server-2");
  server_vector.push_back(server2);

  ServerManager server_manager(server_vector);
  if (!server_manager.SELECT_initializeServers()) {
    server_manager.closeServerSockets();
    return 1;
  }
  server_manager.SELECT_runServers();
  return 0;
}
