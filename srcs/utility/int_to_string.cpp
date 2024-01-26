#include "utility.hpp"

/*! \brief returns string representation of integer argument
*       
*	Uses string stream to conver arguent to string object.
*   @param integer which will be converted into a string.
*   @return a string that is built from the passed argument.
*
*/
std::string	intToString(int integer) {
	
	std::stringstream ss;
	ss << integer;

	return ss.str();
}
