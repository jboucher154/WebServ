#include "utility.hpp"
#include <sstream>
#include <iomanip>

std::string	urlEncode( std::string to_encode ) {

	std::ostringstream	encoded_stream;

	encoded_stream	<< std::hex << std::uppercase << std::setfill('0');
	for (std::string::iterator it = to_encode.begin(); it != to_encode.end(); it++) {
		if (std::isalnum(*it) || *it == '-' || *it == '_' || *it == '.' || *it == '~') {
			encoded_stream << *it;
		}
		else {
			encoded_stream << '%' << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(*it));
		}
	}
	return (encoded_stream.str());
}