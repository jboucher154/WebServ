#include "utility.hpp"

/*! \brief returns string representation of integer argument
*       
*	Uses string stream to conver arguent to string object.
*
*/
std::string	intToString(int integer) {
	
	std::stringstream ss;
	ss << integer;

	return ss.str();
}
