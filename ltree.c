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


long long ls(char *dirname, DIR *directory, int indentation, int maxIndentation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType) {
	char *spacetab = " |  ";

	long long dirsize = 0;

	struct dirent *item;
	directory = opendir(dirname);
	struct stat st;

	while ((item = readdir(directory)) != NULL) {
		if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {
			
			printf(CNORM);

			// make indentation
			if (indentation + 1 < maxIndentation) {
				printIndentation(indentation);
			}

			// checking if searched file name
			bool searchFound = false;
			if (strstr(item->d_name, search) && searchEnabled) {
				printf(CBLACK);
				searchFound = true;
			}

			// creating new path
			char temp = '/';
			char newpath[4096];
			strcpy(newpath, dirname);
			strncat(newpath, &temp, 1);
			strcat(newpath, item->d_name);
		
			// check if is folder
			if(item->d_type == DT_DIR) {
				if (indentation + 1 < maxIndentation || searchFound) {
					// fill indentation if hadn't already
					if (indentation + 1 >= maxIndentation) {
						printf("%s", CNORM);
						printIndentation(indentation);
						printf("%s", CBLACK);
					}

					printf("^ %s%s%s%s%s \t\t [ %s ]%s\n", CBOLD, CBLUE_FG, item->d_name, CNORM, CBOLD, newpath, CNORM);
				}

				bool hasPermission = (st.st_mode & S_IRUSR) | (st.st_mode & S_IRGRP);
				// recursive call
				if (hasPermission) dirsize += ls(newpath, directory, indentation + 1, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType);

			} else {
				// file type
				/* int out = stat(newpath, &st); */
				int out = lstat(newpath, &st);
				if (out < 0) {
					fprintf(stderr, "%sStat failed! errno(%d), dir:%s%s\n", CRED, errno, newpath, CNORM);
					return 1;
				}
				char cFileType[63] = "";
				substring(item->d_name, cFileType, findChar(item->d_name, '.') + 1, 512);
				dirsize += (long long)(unsigned long)st.st_size;
				// file type check
				bool isSymLink = S_ISLNK(st.st_mode);
				bool isExec = st.st_mode & S_IXUSR;

				// search in files
				bool found = false;
				if (fileSearchEnabled && (strcmp(fileType, cFileType) == 0 || strcmp(fileType, "") == 0) && !isSymLink) {
					found = findInFile(newpath, fileSearch);
				}
				if (found) {
					printf(CBLACK);
				}

				// print name
				if (indentation + 1 < maxIndentation || found || searchFound) {
					// fill indentation if hadn't already
					if (indentation + 1 >= maxIndentation) {
						printf("%s", CNORM);
						printIndentation(indentation);
						printf("%s", CBLACK);
					}

					int tempLen = normalize((long long)(unsigned long)st.st_size);

					for (int i = 0 ; i < 12 - tempLen ; i++) {
						printf(" ");
					}
					printf("] ");
					
					if (isSymLink) {
						printf("%s%s%s", CRED_FG, item->d_name, CNORM);
					}
					else if (isExec) {
						printf("%s%s%s", CGREEN_FG, item->d_name, CNORM);
					} else {
						printf("%s%s", item->d_name, CNORM);
					}

					if (found || searchFound) {
						printf("%s\t\t [ %s ]%s", CBOLD, newpath, CNORM);
					}
					printf("\n");
				}
			}
			printf(CNORM);
		}
	}
	
	if (indentation < maxIndentation) {
		for (int i = 0 ; i < indentation - 1 ; i++) {
			printf("  | ");
		}
		printf("  |_");
		
		int tempLen = normalize(dirsize);
		for (int i = 0 ; i < 12 - tempLen ; i++) {
			printf(" ");
		}
		printf("]");
		printf("\n");

		// spacer after a directory
		for (int i = 0 ; i < indentation - 1 ; i++) {
			printf("  | ");
		}
		printf("\n");
	}

	closedir(directory);

	return dirsize;

}


int main (int argc, char *argv[]) {

	// INFO: new default is 2
	/* int maxIndentation = 99; */
	int maxIndentation = 2;
	
	bool searchEnabled = false;
	char *search = calloc(2, sizeof(char));
	
	bool fileSearchEnabled = false;
	char *fileSearch = calloc(2, sizeof(char));
	char *fileType = calloc(2, sizeof(char));

	bool wait = false;

	if (argc > 16) {
		fprintf(stderr, "Too many arguments passed !\n");
		return 1;
	} 


	int labelCount = getLabelCount(argc, argv);
	ARG *args = getArgs(argc, argv);
	if (args == NULL) {
		return 1;
	}

	for (int i = 0; i < labelCount; i++) {
		// help
		if (strcmp(args[i].label, LLabels[HELP]) == 0 || strcmp(args[i].label, SLabels[HELP]) == 0) {
			printf(rawHelpString, 
					LLabels[HELP], SLabels[HELP],
					LLabels[SEARCH], SLabels[SEARCH],
					LLabels[FIND], SLabels[FIND],
					LLabels[MAX_INDEX], SLabels[MAX_INDEX],
					LLabels[LEAVE_OPEN], SLabels[LEAVE_OPEN],
					LLabels[VERSION], SLabels[VERSION]
					);
			return 0;
		}
		// file name search
		else if (strcmp(args[i].label, LLabels[SEARCH]) == 0 || strcmp(args[i].label, SLabels[SEARCH]) == 0) {
			if (args[i].value == NULL) {
				fprintf(stderr, "Missing value for search \n");
				return 1;
			}
			searchEnabled = true;
			setSearch(args[i].value, search);
		}
		// in-file search
		else if (strcmp(args[i].label, LLabels[FIND]) == 0 || strcmp(args[i].label, SLabels[FIND]) == 0) {
			if (args[i].value == NULL) {
				fprintf(stderr, "Missing value for in-file search \n");
				return 1;
			}
			fileSearchEnabled = true;
			int res = setFileSearch(args[i].value, fileSearch, fileType);
			if (res == 1) {
				fprintf(stderr, "Failed to set in-file search \n");
				return 1;
			}
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
		else {
			fprintf(stderr, "Unknown flag: >%s<\n", args[i].label);
			return 1;
		}
	}


	DIR *directory;
	
	printf(".\n");
	ls(".", directory, 0, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType);

	printf(CNORM);

	if (wait) {
		char tempC;
		scanf("%c", &tempC);
	}

	free(args);
	free(search);
	free(fileSearch);
	free(fileType);
	return 0;
}
