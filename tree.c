# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <stdbool.h>
# include <dirent.h>
# include <sys/stat.h>
# include <errno.h>

# include "libs/argLib.h"
# include "libs/common.h"
# include "stringEx/stringEx.h"
# include "tree.h"
# include "libs/specific.h"


int main (int argc, char *argv[]) {

	// INFO: new default is 2
	/* int maxIndentation = 99; */
	int maxIndentation = 2;
	
	bool searchEnabled = false;
	char *search = calloc(2, sizeof(char));
	
	bool fileSearchEnabled = false;

	SEARCH_POINTERS sp = {
		.fileSearch = calloc(2, sizeof(char)),
		.fileType = calloc(2, sizeof(char))
	};

	bool wait = false;
	bool displayAllFiles = false;
	bool use_old_style = false;

	int arg_start = DEFAULT_ARG_START;
	char default_starting_directory[] = ".";
	char *starting_directory = calloc(1, sizeof(char));
	strncpy(starting_directory, default_starting_directory, 1);

	// careful when adding falgs in future, might need to be increased
	// current max is: 13
	if (argc > 16) {
		fprintf(stderr, "Too many arguments passed !\n");
		return 1;
	} 

	// check if first argument is path and skip it if is
	if (argc > 1 && argv[DEFAULT_ARG_START][0] != '-') {
		arg_start += 1;

		int new_len = strlen(argv[DEFAULT_ARG_START]);
		char *tmp = realloc(starting_directory, new_len);

		if (tmp == NULL) {
			fprintf(stderr, "Failed to realloc memory, exiting...\n");
			return 1;
		}

		starting_directory = tmp;
		strncpy(starting_directory, argv[DEFAULT_ARG_START], new_len);
	}


	int labelCount = getLabelCount(argc, argv, arg_start);
	ARG *args = getArgs(argc, argv, arg_start);
	if (args == NULL) {
		return 1;
	}

	for (int i = 0; i < labelCount; i++) {
		// help
		if (strcmp(args[i].label, LLabels[HELP]) == 0 || strcmp(args[i].label, SLabels[HELP]) == 0) {
			printf(rawHelpString, 
					LLabels[HELP], SLabels[HELP],
					LLabels[SEARCH_IN_FILE], SLabels[SEARCH_IN_FILE],
					LLabels[FIND], SLabels[FIND],
					LLabels[MAX_INDEX], SLabels[MAX_INDEX],
					LLabels[LEAVE_OPEN], SLabels[LEAVE_OPEN],
					LLabels[VERSION], SLabels[VERSION],
					LLabels[ALL], SLabels[ALL],
					LLabels[OLD_STYLE], SLabels[OLD_STYLE]
					);
			return 0;
		}
		// in-file search
		else if (strcmp(args[i].label, LLabels[SEARCH_IN_FILE]) == 0 || strcmp(args[i].label, SLabels[SEARCH_IN_FILE]) == 0) {
			if (args[i].value == NULL) {
				fprintf(stderr, "Missing value for in-file search \n");
				return 1;
			}
			fileSearchEnabled = true;
			int res = setFileSearch(args[i].value, &sp);
			if (res == 1) {
				fprintf(stderr, "Failed to set in-file search \n");
				return 1;
			}
		}
		// find file name 
		else if (strcmp(args[i].label, LLabels[FIND]) == 0 || strcmp(args[i].label, SLabels[FIND]) == 0) {
			if (args[i].value == NULL) {
				fprintf(stderr, "Missing value for find\n");
				return 1;
			}
			searchEnabled = true;
			search = setSearch(args[i].value, search);
		}
		// max-indentation also known as max index
		else if (strcmp(args[i].label, LLabels[MAX_INDEX]) == 0 || strcmp(args[i].label, SLabels[MAX_INDEX]) == 0) {
			if (args[i].value == NULL) {
				fprintf(stderr, "Missing value for max index/indentation \n");
				return 1;
			}
			maxIndentation = setIndentation(args[i].value);  
		}
		// leave open after finish = aka wait for user input to close
		else if (strcmp(args[i].label, LLabels[LEAVE_OPEN]) == 0 || strcmp(args[i].label, SLabels[LEAVE_OPEN]) == 0) {
			wait = true;
		}
		// version info
		else if (strcmp(args[i].label, LLabels[VERSION]) == 0 || strcmp(args[i].label, SLabels[VERSION]) == 0) {
			printf("TermTree v%s\n", version);
			return 0;
		}
		// display all items, not only the searched ones
		else if (strcmp(args[i].label, LLabels[ALL]) == 0 || strcmp(args[i].label, SLabels[ALL]) == 0) {
			displayAllFiles = true;
		}
		// use old visual style 
		else if (strcmp(args[i].label, LLabels[OLD_STYLE]) == 0 || strcmp(args[i].label, SLabels[OLD_STYLE]) == 0) {
			use_old_style = true;
		}
		// Unknown flag
		else {
			fprintf(stderr, "Unknown flag: >%s<\n", args[i].label);
			return 1;
		}
	}


	DIR *directory;
	
	printf(".\n");
	ls(starting_directory, directory, 0, maxIndentation, search, searchEnabled, sp.fileSearch, fileSearchEnabled, sp.fileType, displayAllFiles, use_old_style);

	printf(CNORM);

	if (wait) {
		char tempC;
		scanf("%c", &tempC);
	}

	free(args);
	free(search);
	free(sp.fileSearch);
	free(sp.fileType);
	free(starting_directory);
	return 0;
}
