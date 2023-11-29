#include "utility.hpp"


/*! \brief Delete an allocated NULL terminating c-style string array.
*       
*	Use only with NULL terminating array, otherwise will cause an error!
*/
void	deleteAllocatedCStringArray(char** array) {

	std::cout << "deleteAllocatedCstringArray 1" << std::endl;
	if (array == NULL)
		return;

	std::cout << "deleteAllocatedCstringArray 2" << std::endl;
	for (int i = 0; array[i]; ++i) {
		std::cout << "deleteAllocatedCstringArray 3.1" << std::endl;
		memset(array[i], 0, std::strlen(array[i]));
		delete [] array[i];
		std::cout << "deleteAllocatedCstringArray 3.2" << std::endl;
	}
	std::cout << "deleteAllocatedCstringArray 4" << std::endl;
	delete [] array;
	std::cout << "deleteAllocatedCstringArray 5" << std::endl;

	array = NULL;	// not sure if this will do anything, check later
}