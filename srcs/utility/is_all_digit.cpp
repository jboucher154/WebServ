#include "utility.hpp"

/*! \brief returns bool indicating if string passed is composed of all digits
*       
*	Returns bool indicating if string passed is composed of all digits.
*   @param s, a refrance to a string which will be probed for possible non digit characters.
*   @return true/false a bool indicating if the string was all digits or not.
*
*/
bool isAllDigit(const std::string& s) {
    
    std::string::const_iterator it = s.begin();
    
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}
