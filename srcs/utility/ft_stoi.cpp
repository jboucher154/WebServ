#include "utility.hpp"

int ft_stoi(const std::string& str)
{
    int num = 0;
    std::istringstream ss(str);

    ss >> num;
    int integer = num;
    std::stringstream checkSS;
	checkSS << integer;
    if ( checkSS.str().compare ( str ) != 0 )
        throw (std::invalid_argument ( "input should be whithin the range of unsigned int" ) );
    return num;
}