#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringEx.h"

void setSearch(char *argv, char *search) {
	char *temp = realloc(search, (strlen(argv) - 2) * sizeof(char));
	if (temp != NULL) {
		search = temp;
	}
	substring(argv, search, 2, 512);
}

int setIndentation(char *argv) {
	char temp[64] = "";

	substring(argv, temp, 2, 512);
	int maxIndentation = atoi(temp) + 1;

	if (maxIndentation < 2) {
		maxIndentation = 2;
	}

	return maxIndentation;
}

int setFileSearch(char *argv, char *fileSearch, char *fileType) {
	if (findChar(argv, '/') == 0) {
		printf("Missing \"/\" \n");
		return 1;
	}

	char *temp = realloc(fileSearch, (strlen(argv) - 2) * sizeof(char));
	if (temp != NULL) {
		fileSearch = temp;
	}


	substring(argv, fileSearch, 2, findChar(argv, '/'));
	if (strlen(fileSearch) <= 0) {
		printf("Missing search item !");
		return 1;
	}

	char *tempT = realloc(fileType, (strlen(argv) - findChar(argv, '/') + 1) * sizeof(char));
	if (tempT != NULL) {
		fileType = tempT;
	}
	substring(argv, fileType, findChar(argv, '/') + 1, 512);

	return 0;
}
