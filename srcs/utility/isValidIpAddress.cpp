#include "utility.hpp"

bool isValidIPAddress(const char* ipAddress) {

    struct in_addr addr;
    
    // Attempt to convert the IP address from string to binary form
    if (inet_pton(AF_INET, ipAddress, &addr) <= 0) {
		Logger::log(E_ERROR, COLOR_RED, "error while parsing given IP address: %s is not a valid IPv4 address", ipAddress);
        return false;
    }

    // IP address is valid
    return true;
}