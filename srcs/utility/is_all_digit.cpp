#include "utility.hpp"

/*! \brief returns bool indicating if string passed is composed of all digits
*       
*	Returns bool indicating if string passed is composed of all digits.
*
*/
bool isAllDigit(const std::string& s) {
    
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}