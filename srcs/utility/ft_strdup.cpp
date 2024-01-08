#include "utility.hpp"

/*! \brief returns allocated copy of c-style string argument.
*       
*	Returns allocated copy of c-style string argument. Will return NULL if 
*	allocation fails.
*
*/
char*	ft_strdup(const char* str) {
	
	char*	str_copy = NULL;
	int		i = 0;

	try {
		if (str == NULL)
			throw std::invalid_argument("argument is NULL");
		while (str[i])
			i++;
		str_copy = new char[i + 1];
		for (int j = 0; j < i; ++j) {
			str_copy[j] = str[j];
		}
		str_copy[i] = '\0';
		return str_copy;
	}
	catch (std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "strdup: %s", e.what());
		return NULL;
	}
}