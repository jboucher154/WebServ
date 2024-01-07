#include "utility.hpp"

/*! \brief converts a string to an integer. May throw exception for over/ under 
*               flow or if empty string is passed
*       
*	Converts a string to an integer. May throw invalid_argument exception for over/ under 
*   flow or if empty string is passed.
*
*/
int ft_stoi(const std::string& str) {

    int num = 0;
    std::istringstream ss(str);

    ss >> num;
    int integer = num;
    std::stringstream checkSS;
	checkSS << integer;
    if ( checkSS.str().compare ( str ) != 0 )
        throw (std::invalid_argument ( "input should be whithin the range of an int" ) );
    return num;
}
