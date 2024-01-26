#include "utility.hpp"

/*! \brief returns bool indicating if c-style string is a valid ip address
*       
*	Returns bool indicating if c-style string is a valid ip address. Attempts
*   to convert the IP address from string to binary form using iner_pton().
*   @param ipAddress which will be checked for validity as an ip address.
*   @return trur/false a bool indicating if the ip address was a valid one or not.
*
*/
bool isValidIpAddress(const char* ipAddress) {

    struct in_addr addr;
    
    if (inet_pton(AF_INET, ipAddress, &addr) <= 0) {
		Logger::log(E_ERROR, COLOR_RED, "error while parsing given IP address: %s is not a valid IPv4 address", ipAddress);
        return false;
    }

    return true;
}
