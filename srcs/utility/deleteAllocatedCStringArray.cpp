#include "utility.hpp"


/*! \brief Delete an allocated NULL terminating c-style string array.
*       
*	Use only with NULL terminating array, otherwise will cause an error!
*/
void	deleteAllocatedCStringArray(char** array) {

	if (array == NULL)
		return;

	for (int i = 0; array[i]; ++i) {
		memset(array[i], 0, std::strlen(array[i]));
		delete [] array[i];
	}
	delete [] array;

	array = NULL;	// not sure if this will do anything, check later
}