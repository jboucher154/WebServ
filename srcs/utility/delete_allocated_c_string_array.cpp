#include "utility.hpp"


/*! \brief Delete an allocated NULL terminating c-style string array.
*   
*	Delete an allocated NULL terminating c-style string array and sets to NULL.    
*	Use only with a NULL terminating array, otherwise will cause a segfault!
*/
void	deleteAllocatedCStringArray(char** array) {

	if (array == NULL)
		return;
	for (int i = 0; array[i]; ++i) {
		memset(array[i], 0, std::strlen(array[i]));
		delete [] array[i];
	}

	delete [] array;
	array = NULL;
}