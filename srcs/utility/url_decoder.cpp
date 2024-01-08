#include "utility.hpp"

/*! \brief returns url decoded inut string, leaving '/' undecoded
*       
*	Returns url decoded input string leaving '/' undecoded. Used to decode
*   uri from client.
*
*/
std::string urlDecode(const std::string& input) {

    std::ostringstream decoded;
    decoded.fill('0');

    for (size_t i = 0; i < input.length(); ++i) {
        char ch = input[i];

        if (ch == '%' && i + 2 < input.length() &&
                   isxdigit(static_cast<unsigned char>(input[i + 1])) &&
                   isxdigit(static_cast<unsigned char>(input[i + 2]))) {
            int value;
            std::istringstream hexStream(input.substr(i + 1, 2));
            hexStream >> std::hex >> value;
            decoded << static_cast<char>(value);
            i += 2;
        } else {
            decoded << ch;
        }
    }
    return decoded.str();
}