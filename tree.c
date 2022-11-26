#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>

#include "libs/argLib.h"
#include "libs/stringEx.h"

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

int normalize(int bytes) {
	char buf[64] = "";
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

int ls(char *dirname, DIR *directory, int indentation, int maxIndentation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType, HANDLE cTerm) {
	struct dirent *item;
	directory = opendir(dirname);
	struct stat st;

	int dirsize = 0;
	
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
					dirsize += st.st_size;

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
						int tempLen = normalize(st.st_size);

						for (int i = 0 ; i < 10 - tempLen ; i++) {
							printf(" ");
						}
						printf("] ");
						
						printf("%s\n", item->d_name);
					}
				}
				resetColor(cTerm);
			}
		}
		
		if (indentation + 1 < maxIndentation) {
			for (int i = 0 ; i < indentation - 1 ; i++) {
				printf("  | ");
			}
			printf("  |_");
			
			int tempLen = normalize(dirsize);
			for (int i = 0 ; i < 10 - tempLen ; i++) {
				printf(" ");
			}
			printf("]");

			printf("\n");
		}

		closedir(directory);
	}

	return dirsize;
}


int main (int argc, char *argv[]) {


	char *helpString = 
		"Help for TermTree \n"
		"\t -h \n\t\t-shows help\n"
		"\t -ftext_to_find/file_type(leave empty for all file types) \n\t\t-search in file \n"
		"\t -sfilename \n\t\t-search for specific filename\n"
		"\t -i<number> \n\t\t-replace <number> with a whole number specifing how deep into the folders hould the seach go\n"
		"\t -L \n\t\t-wait for a keypress to close the program\n";



	int maxIndentation = 99;
	
	bool searchEnabled = false;
	char *search = calloc(2, sizeof(char));
	
	bool fileSearchEnabled = false;
	char *fileSearch = calloc(2, sizeof(char));
	char *fileType = calloc(2, sizeof(char));

	bool wait = false;

	if (argc > 5) {
		printf("Too many arguments passed !");

		return 1;
	} 
	// arguments passed
	if (argc > 1) {
		for (int i = 1 ; i < argc ; i++) {
			char flag[2] = "";
			substring(argv[i], flag, 0, 2);

			if (strcmp(flag, "-S") == 0 || strcmp(flag, "-s") == 0) {
				searchEnabled = true;
				setSearch(argv[i], search);

			} else if (strcmp(flag, "-I") == 0 || strcmp(flag, "-i") == 0) {
				maxIndentation = setIndentation(argv[i]);  

			} else if (strcmp(flag, "-F") == 0 || strcmp(flag, "-f") == 0) {
				fileSearchEnabled = true;
				int res = setFileSearch(argv[i], fileSearch, fileType);
				if (res == 1) {
					return 1;
				}

			} else if (strcmp(flag, "-L") == 0 || strcmp(flag, "-l") == 0) {
				wait = true;
			} else if (strcmp(flag, "-H") == 0 || strcmp(flag, "-h") == 0) {
				printf("%s", helpString);
				return 0;
			} else {
				printf("unknown flag: %s", flag);
				return 1;
			}
			
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

	free(search);
	free(fileSearch);
	free(fileType);
	resetColor(cTerm);
	return 0;
}
