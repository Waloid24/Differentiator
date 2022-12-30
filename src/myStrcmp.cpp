#include "myStrcmp.h"

int myStrcmp (const char * string1, const char * string2)
{
	int i = 0, j = 0;
	for (; string1[i] != '\0' && string2[j] != '\0'; i++, j++)
	{
		while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;
		while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;
		if (tolower(string1[i]) == tolower(string2[j]))
			continue;
		return (tolower(string1[i]) - tolower(string2[j]));
	}

	while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;

	while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;

	return (tolower(string1[i]) - tolower(string2[j]));
}