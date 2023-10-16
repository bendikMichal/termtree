# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <math.h>
# include <dirent.h>
# include <sys/stat.h>
# include <windows.h>
# include <errno.h>

# include "libs/argLib.h"
# include "stringEx/stringEx.h"
# include "tree.h"

// coloring
void allColors(HANDLE cTerm) {
	for (int i = 0 ; i < 256; i++) {
		SetConsoleTextAttribute(cTerm, i);

		printf("\nTree %d", i);
	}
}

void resetColor(HANDLE cTerm) {
	SetConsoleTextAttribute(cTerm, 15);
}
//

bool findInFile (char *filename, char *item) {
	FILE *file;
	fopen_s(&file, filename, "r");
	
	if (file == NULL) {
		fclose(file);
		return false;
	}

	char line[2048];
	while(fgets(line, sizeof(line), file)) {
		if (strstr(line, item)) {
			fclose(file);
			return true;
		}
	}

	fclose(file);
	return false;
}

int normalize(long long bytes) {
	char buf[128] = "";
	if (bytes >= pow(10, 12)) {
		sprintf(buf, "[ %.1f tb ", bytes / pow(10, 12));
		printf("%s", buf);
		return strlen(buf);
	} else if (bytes >= pow(10, 9)) {
		sprintf(buf, "[ %.1f gb ", bytes / pow(10, 9));
		printf("%s", buf);
		return strlen(buf);
	} else if (bytes >= pow(10, 6)) {
		sprintf(buf, "[ %.1f mb ", bytes / pow(10, 6));
		printf("%s", buf);
		return strlen(buf);
	} else if (bytes >= pow(10, 3)) {
		sprintf(buf, "[ %.1f kb ", bytes / pow(10, 3));
		printf("%s", buf);
		return strlen(buf);
	} else if (bytes < pow(10, 3)) {
		sprintf(buf, "[ %d b  ", bytes);
		printf("%s", buf);
		return strlen(buf);
	} 
}

long long ls(char *dirname, DIR *directory, int indentation, int maxIndentation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType, HANDLE cTerm) {
	struct dirent *item;
	directory = opendir(dirname);
	struct stat st;

	long long dirsize = 0;
	
	if (directory) {
		while ((item = readdir(directory)) != NULL) {
			if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {

				printf("%s", CNORM);

				// make indentation
				if (indentation + 1 < maxIndentation) {
					for (int i = 0 ; i < indentation ; i++) {
						printf("  | ");
					}
				}

				// checking if searched file name
				if (strstr(item->d_name, search) && searchEnabled) {
					printf("%s", CBLACK);
				}

				// creating new path
				char temp = '/';
				char newpath[4096];
				strcpy(newpath, dirname);
				strncat(newpath, &temp, 1);
				strcat(newpath, item->d_name);
			
				int out = stat(newpath, &st);
				if (out < 0) {
					fprintf(stderr, "%sStat failed! errno(%d), dir:%s%s\n", CRED, errno, newpath, CNORM);
					return 1;
				}

				// check if is folder
				if(S_ISDIR(st.st_mode) ) {
					if (indentation + 1 < maxIndentation) {
						printf("^ %s%s%s%s%s \t\t [ %s ]%s\n", CBOLD, CBLUE_FG, item->d_name, CNORM, CBOLD, newpath, CNORM);
					}
					// recursive call
					dirsize += ls(newpath, directory, indentation + 1, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType, cTerm);

				} else {
					// file type
					char cFileType[64] = "";
					substring(item->d_name, cFileType, findChar(item->d_name, '.') + 1, 512);
					dirsize += (long long)(unsigned long)st.st_size;

					// file type check
					bool isExec = st.st_mode & S_IXUSR;

					// search in files
					bool found = false;
					if (fileSearchEnabled && (strcmp(fileType, cFileType) == 0 || strcmp(fileType, "") == 0)) {
						found = findInFile(newpath, fileSearch);
					}
					if (found) {
						printf("%s", CBLACK);
					}

					// print name
					if (indentation + 1 < maxIndentation) {
						int tempLen = normalize((long long)(unsigned long)st.st_size);

						for (int i = 0 ; i < 10 - tempLen ; i++) {
							printf(" ");
						}
						printf("] ");
						
						if (isExec) {
							printf("%s%s%s\n", CGREEN_FG, item->d_name, CNORM);
						} else {
							printf("%s", item->d_name);
						}
						printf("%s", CNORM);
						printf("\n");
					}
				}
				printf("%s", CNORM);
			}
		}
		
		if (indentation < maxIndentation) {
			for (int i = 0 ; i < indentation - 1 ; i++) {
				printf("  | ");
			}
			printf("  |_");
			
			int tempLen = normalize(dirsize);
			for (int i = 0 ; i < 10 - tempLen ; i++) {
				printf(" ");
			}
			printf("]");

			printf("%s", CNORM);
			printf("\n");
		}

		closedir(directory);
	}

	return dirsize;
}


int main (int argc, char *argv[]) {

	char LLabels[][MAX_LABEL_LEN] = {
		"--help",
		"--search",
		"--find",
		"--max-index",
		"--leave-open",
		"--version"

	};
	char SLabels[][MAX_LABEL_LEN] = {
		"-h",
		"-s",
		"-f",
		"-i",
		"-L",
		"-v"
	};

	char *rawHelpString = 
		"Help for TermTree \n"
		"\t %s, %s \n\t\t- shows help\n"
		"\t %s, %s <text>/<file_type> \n\t\t- search in file, if <file_type> is empty, search will be in all files\n"
		"\t %s, %s <file/folder name> \n\t\t- search for a specific file/folder name\n"
		"\t %s. %s <number> \n\t\t- replace <number> with a whole number specifing how deep into the folders should the seach go\n"
		"\t %s, %s \n\t\t- wait for a keypress to close the program\n"
		"\t %s, %s \n\t\t- display version number\n";



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
		printf("Too many arguments passed !");
		return 1;
	} 


	int labelCount = getLabelCount(argc, argv);
	ARG *args = getArgs(argc, argv);

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

	HANDLE cTerm = GetStdHandle(STD_OUTPUT_HANDLE);
	
	DIR *directory;
	
	printf(".\n");
	ls(".", directory, 0, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType, cTerm);

	if (wait) {
		char tempC;
		scanf("%c", &tempC);
	}

	free(args);
	free(search);
	free(fileSearch);
	free(fileType);
	printf("%s", CNORM);
	return 0;
}
