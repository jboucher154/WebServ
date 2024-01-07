#include "utility.hpp"

/*! \brief This function gets the current date and time as a string
*
*	Used to timestamp the logs with current time and date with localtime.
*/
std::string	getTimestampString( void ) {
	
	time_t	now = time(0);
	struct tm tstruct;
	char	buf[80];

	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

