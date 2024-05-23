# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "../tree.h"
# include "argLib.h"
# include "../stringEx/stringEx.h"

// don't forget to free returned value
ARG* getArgs(int argc, char *argv[], int arg_start) {
	ARG *args = (ARG *) calloc(16, sizeof(ARG));
	int nlabel = -1;

	for (int i = arg_start; i < argc; i++) {

		if (strlen(argv[i]) >= 2 && argv[i][0] == '-' && argv[i][1] == '-') {
			nlabel ++;
			int splitIndex = findChar(argv[i], '=');

			if (splitIndex >= 0) {
				substring(argv[i], args[nlabel].al, 0, splitIndex);
				substring(argv[i], args[nlabel].av, splitIndex + 1, 2047);
				args[nlabel].label = args[nlabel].al;
				args[nlabel].value = args[nlabel].av;

			} else {
				args[nlabel].label = argv[i];
				args[nlabel].value = NULL;
			}

		} else if (argv[i][0] == '-') {

			nlabel ++;
			args[nlabel].label = argv[i];
			args[nlabel].value = NULL;

		} else {

			if (nlabel < 0) {
				fprintf(stderr, "Invalid arguments!\n Tip: Check the order of arguments and look for missing \"-\"\n");
				free(args);
				return NULL;
			}
			args[nlabel].value = argv[i];
		}
	}
	return args;
}

// returns count of labels
int getLabelCount(int argc, char *argv[], int arg_start) {
	int nlabel = 0;
	char pre[2];
	for (int i = arg_start; i < argc; i++) {
		if (argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-')) {
			nlabel ++;
		}
	}
	return nlabel;
}

char* setSearch(char *argv, char *search) {
	char *temp = realloc(search, strlen(argv) * sizeof(char));
	if (temp != NULL) {
		search = temp;
		strcpy(search, argv);
		return search;
	}
	/* substring(argValue, search, 0, 512); */
	strcpy(search, "");

	return search;
}

int setIndentation(char *argv) {
	int maxIndentation = atoi(argv) + 1;

	if (maxIndentation < 2) maxIndentation = 2;
	return maxIndentation;
}

int setFileSearch(char *argv, SEARCH_POINTERS* sp) {
	if (findChar(argv, '/') == 0) {
		printf("Missing \"/\" \n");
		return 1;
	}

	char *temp = realloc(sp->fileSearch, strlen(argv) * sizeof(char));
	if (temp != NULL) {
		sp->fileSearch = temp;
	}
	else {
		printf("Failed to alloc memory for infile search!");
		return 1;
	}

	substring(argv, sp->fileSearch, 0, findChar(argv, '/'));
	if (strlen(sp->fileSearch) <= 0) {
		printf("Missing search item !");
		return 1;
	}

	char *tempT = realloc(sp->fileType, (strlen(argv) - findChar(argv, '/') + 1) * sizeof(char));
	if (tempT != NULL) {
		sp->fileType = tempT;
	}
	else {
		printf("Failed to alloc memory for infile search file type!");
		return 1;
	}

	substring(argv, sp->fileType, findChar(argv, '/') + 1, 512);

	return 0;
}
