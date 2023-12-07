#include "utility.hpp"

std::string	intToString(int integer) {
	
	std::stringstream ss;
	ss << integer;

	return (ss.str());
}
