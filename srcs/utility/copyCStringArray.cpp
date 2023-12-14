#include "utility.hpp"


/*! \brief Create an allocated copy of a NULL-terminating array of c-style strings.
*       
*	Use only with NULL terminating array, otherwise will cause an error!
*/
char**		copyCStringArray(char** array) {

	char**	copy_array = NULL;
	int		i = 0;

	if (array == NULL)
		return NULL;

	while (array[i])
		i++;

	try {
		copy_array = new char*[i + 1];

		for (int j = 0; j < i; ++j) {
			copy_array[i] = ft_strdup(array[i]);
		}
		copy_array[i] = NULL;
	
		return copy_array;

	} catch (std::exception& e) {
		Logger::log(E_ERROR, COLOR_RED, "copy_c_string_array: %s", e.what());
		deleteAllocatedCStringArray(copy_array);
		return NULL;
	}

}
