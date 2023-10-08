# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <math.h>
# include <dirent.h>
# include <sys/stat.h>
# include <windows.h>

# include "libs/argLib.h"
# include "stringEx/stringEx.h"
# include "tree.h"

// use to build : gcc -o ttree tree.c libs/stringex.c libs/argLib.c

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

				resetColor(cTerm);

				// make indentation
				if (indentation + 1 < maxIndentation) {
					for (int i = 0 ; i < indentation ; i++) {
						printf("  | ");
					}
				}

				// checking if searched file name
				if (strstr(item->d_name, search) && searchEnabled) {
					SetConsoleTextAttribute(cTerm, 0xF0);
				}

				// creating new path
				char temp = '/';
				char newpath[512];
				strcpy(newpath, dirname);
				strncat(newpath, &temp, 1);
				strcat(newpath, item->d_name);

				// file type
				stat(newpath, &st);
				char cFileType[64] = "";
				substring(item->d_name, cFileType, findChar(item->d_name, '.') + 1, 512);
			
				// check if is folder
				if(S_ISDIR(st.st_mode) ) {
					if (indentation + 1 < maxIndentation) {
						printf("^ %s\n", item->d_name);
					}
					// recursive call
					dirsize += ls(newpath, directory, indentation + 1, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType, cTerm);

				} else {
					dirsize += (long long)(unsigned long)st.st_size;

					// search in files
					bool found = false;
					if (fileSearchEnabled && (strcmp(fileType, cFileType) == 0 || strcmp(fileType, "") == 0)) {
						found = findInFile(newpath, fileSearch);
					}
					if (found) {
						SetConsoleTextAttribute(cTerm, 0xF0);
					}

					// print name
					if (indentation + 1 < maxIndentation) {
						int tempLen = normalize((long long)(unsigned long)st.st_size);

						for (int i = 0 ; i < 10 - tempLen ; i++) {
							printf(" ");
						}
						printf("] ");
						
						printf("%s", item->d_name);
						resetColor(cTerm);
						printf("\n");
					}
				}
				resetColor(cTerm);
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

			resetColor(cTerm);
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
		"--leave-open"

	};
	char SLabels[][MAX_LABEL_LEN] = {
		"-h",
		"-s",
		"-f",
		"-i",
		"-L"
	};

	char *rawHelpString = 
		"Help for TermTree \n"
		"\t %s / %s \n\t\t- shows help\n"
		"\t %s / %s <<text_to_find>/<file_type>> \n\t\t- search in file, if <file_type> is empty, search will be in all files\n"
		"\t %s / %s <filename> \n\t\t- search for a specific filename\n"
		"\t %s / %s <number> \n\t\t- replace <number> with a whole number specifing how deep into the folders should the seach go\n"
		"\t %s / %s \n\t\t- wait for a keypress to close the program\n";



	int maxIndentation = 99;
	
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
		if (strcmp(args[i].label, LLabels[0]) == 0 || strcmp(args[i].label, SLabels[0]) == 0) {
			printf(rawHelpString, 
					LLabels[0], SLabels[0],
					LLabels[1], SLabels[1],
					LLabels[2], SLabels[2],
					LLabels[3], SLabels[3],
					LLabels[4], SLabels[4]
					);
			return 0;
		}
		// file name search
		else if (strcmp(args[i].label, LLabels[1]) == 0 || strcmp(args[i].label, SLabels[1]) == 0) {
			if (args[i].value == NULL) {
				printf("Missing value for search \n");
				return 1;
			}
			searchEnabled = true;
			setSearch(args[i].value, search);
		}
		// in-file search
		else if (strcmp(args[i].label, LLabels[2]) == 0 || strcmp(args[i].label, SLabels[2]) == 0) {
			if (args[i].value == NULL) {
				printf("Missing value for in-file search \n");
				return 1;
			}
			fileSearchEnabled = true;
			int res = setFileSearch(args[i].value, fileSearch, fileType);
			if (res == 1) {
				printf("Failed to set in-file search \n");
				return 1;
			}
		}
		// max-indentation also known as max index
		else if (strcmp(args[i].label, LLabels[3]) == 0 || strcmp(args[i].label, SLabels[3]) == 0) {
			if (args[i].value == NULL) {
				printf("Missing value for max index/indentation \n");
				return 1;
			}
			maxIndentation = setIndentation(args[i].value);  
		}
		// leave open after finish = aka wait for user input to close
		else if (strcmp(args[i].label, LLabels[4]) == 0 || strcmp(args[i].label, SLabels[4]) == 0) {
			wait = true;
		}
		else {
			printf("Unknown flag: %s \n", args[i].label);
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
	resetColor(cTerm);
	return 0;
}
