#include "utility.hpp"

std::string	int_to_string(int integer) {
	std::stringstream ss;
	ss << integer;

	return (ss.str());
}