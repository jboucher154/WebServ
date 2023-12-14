#include "utility.hpp"

char*	ft_strdup(const char* str) {
	char*	str_copy = NULL;
	int		i = 0;

	if (str == NULL)
		return NULL;
	
	while (str[i])
		i++;
	
	str_copy = new char[i + 1];

	for (int j = 0; j < i; ++j) {
		str_copy[j] = str[j];
	}

	str_copy[i] = '\0';

	return str_copy;
}