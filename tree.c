#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>

#include "libs/stringEx.h"

// use to build : gcc -o ttree tree.c libs/stringex.c

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
	bool found = false;
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

int ls(char *dirname, DIR *directory, int intedation, int maxIntendation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType, HANDLE cTerm) {
	struct dirent *item;
	directory = opendir(dirname);
	struct stat st;

	int dirsize = 0;
	
	if (directory) {
		while ((item = readdir(directory)) != NULL) {
			if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {

				resetColor(cTerm);

				// make intedation

				if (intedation + 1 < maxIntendation) {
					for (int i = 0 ; i < intedation ; i++) {
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

				// check if is folder
				stat(newpath, &st);
				// file type
				char cFileType[64] = "";
				substring(item->d_name, cFileType, findChar(item->d_name, '.') + 1, 512);
			

				if(S_ISDIR(st.st_mode) ) {
					if (intedation + 1 < maxIntendation) {
						printf("^ %s\n", item->d_name);
					}
					// recursive call
					dirsize += ls(newpath, directory, intedation + 1, maxIntendation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType, cTerm);

				} else {
					dirsize += st.st_size;

					bool found = false;
					if (fileSearchEnabled && (strcmp(fileType, cFileType) == 0 || strcmp(fileType, "") == 0)) {
						found = findInFile(newpath, fileSearch);
					}
					if (found) {
						SetConsoleTextAttribute(cTerm, 0xF0);
					}

					// print name
					if (intedation + 1 < maxIntendation) {
						int tempLen = normalize(st.st_size);

						for (int i = 0 ; i < 10 - tempLen ; i++) {
							printf(" ");
						}
						printf("]");
						
						for (int i = 0 ; i < 10 - tempLen ; i++) {
							printf(" ");
						}
						printf("%s\n", item->d_name);
					}
				}
				resetColor(cTerm);
			}
		}
		
		if (intedation + 1 < maxIntendation) {
			for (int i = 0 ; i < intedation - 1 ; i++) {
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
	int maxIntendation = 99;
	
	bool searchEnabled = false;
	char *search = calloc(2, sizeof(char));
	
	bool fileSearchEnabled = false;
	char *fileSearch = calloc(2, sizeof(char));
	char *fileType = calloc(2, sizeof(char));

	if (argc > 4) {
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
				char *temp = realloc(search, (strlen(argv[i]) - 2) * sizeof(char));
				if (temp != NULL) {
					search = temp;
				}
				substring(argv[i], search, 2, 512);

			} else if (strcmp(flag, "-I") == 0 || strcmp(flag, "-i") == 0) {
				char temp[64] = "";
				
				substring(argv[i], temp, 2, 512);
				maxIntendation = atoi(temp) + 1;

				if (maxIntendation < 2) {
					maxIntendation = 2;
				}

			} else if (strcmp(flag, "-F") == 0 || strcmp(flag, "-f") == 0) {
				if (findChar(argv[i], '/') == 0) {
					printf("Missing \"/\"");
					return 1;
				}

				fileSearchEnabled = true;
				char *temp = realloc(fileSearch, (strlen(argv[i]) - 2) * sizeof(char));
				if (temp != NULL) {
					fileSearch = temp;
				}


				substring(argv[i], fileSearch, 2, findChar(argv[i], '/'));
				if (strlen(fileSearch) <= 0) {
					printf("Missing search item !");
					return 1;
				}
				
				char *tempT = realloc(fileType, (strlen(argv[i]) - findChar(argv[i], '/') + 1) * sizeof(char));
				if (tempT != NULL) {
					fileType = tempT;
				}
				substring(argv[i], fileType, findChar(argv[i], '/') + 1, 512);

			} else {
				printf("unknown flag: %s", flag);
				return 1;
			}
			
		}
	}


	HANDLE cTerm = GetStdHandle(STD_OUTPUT_HANDLE);
	
	DIR *directory;
	
	printf(".\n");
	ls(".", directory, 0, maxIntendation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType, cTerm);


	free(search);
	free(fileSearch);
	free(fileType);
	resetColor(cTerm);
	return 0;
}
