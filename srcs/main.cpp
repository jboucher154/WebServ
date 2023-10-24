// rm later; testing
#include "Logger.hpp"

#include <iostream>
#include <unistd.h>

int  main( int argc, char *argv[]) {

  (void)argv;
  if (argc == 2) {
    std::cout << "parse will be called!" << std::endl;
  }
  Logger::initLogger();
  std::cout << "declaring Logger!" << std::endl;
  std::cout << "after Logger construction!" << std::endl;
  std::cout << "test log()!" << std::endl;
  Logger::log(E_INFO, NULL, "logger message: testing variadic func\tint: %i\tstring: %s", 42, "YOLO");
  std::cout << "waiting 3 sec" << std::endl;
  sleep(3);
  Logger::log(E_ERROR, NULL, "NEWER logger message:\tint: %i\tstring: %s", 3, "AYY");
  sleep(3);

  return (0);
}
