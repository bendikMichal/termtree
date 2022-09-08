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
void setColor(HANDLE cTerm, char *color) {
	if (color == "greenBg"){
		SetConsoleTextAttribute(cTerm, 0x20);
	}else if (color == "green") {
		SetConsoleTextAttribute(cTerm, 0x0a);
	}else if (color == "brown") {
		SetConsoleTextAttribute(cTerm, 0x06);
	}
}

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

void normalize(int bytes) {
	if (bytes >= pow(10, 12)) {
		printf("[ %.1f tb ] ", bytes / pow(10, 12));
	} else if (bytes >= pow(10, 9)) {
		printf("[ %.1f gb ] ", bytes / pow(10, 9));
	} else if (bytes >= pow(10, 6)) {
		printf("[ %.1f mb ] ", bytes / pow(10, 6));
	} else if (bytes >= pow(10, 3)) {
		printf("[ %.1f kb ] ", bytes / pow(10, 3));
	} else if (bytes < pow(10, 3)) {
		printf("[ %d b ] ", bytes);
	} 
}

int ls(char *dirname, DIR *directory, int intedation, int maxIntendation, char *search, bool searchEnabled, HANDLE cTerm) {
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

				// checking if searched file
				if (strstr(item->d_name, search) && searchEnabled) {
					SetConsoleTextAttribute(cTerm, 0x1D);
				}

				// creating new path
				char temp = '/';
				char newpath[512];
				strcpy(newpath, dirname);
				strncat(newpath, &temp, 1);
				strcat(newpath, item->d_name);

				// check if is folder
				stat(newpath, &st);
			

				if(S_ISDIR(st.st_mode) ) {
					if (intedation + 1 < maxIntendation) {
						printf("^ %s\n", item->d_name);
					}
					// recursive call
					dirsize += ls(newpath, directory, intedation + 1, maxIntendation, search, searchEnabled, cTerm);

				} else {
					dirsize += st.st_size;

					// print name
					if (intedation + 1 < maxIntendation) {
						normalize(st.st_size);
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
			normalize(dirsize);
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

	if (argc > 3) {
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
				maxIntendation = atoi(temp);

				if (maxIntendation < 2) {
					maxIntendation = 2;
				}

			}else {
				printf("unknown flag: %s", flag);
				return 1;
			}
			
		}
	}


	HANDLE cTerm = GetStdHandle(STD_OUTPUT_HANDLE);
	
	DIR *directory;
	
	printf(".\n");
	ls(".", directory, 0, maxIntendation, search, searchEnabled, cTerm);


	free(search);
	resetColor(cTerm);
	return 0;
}
