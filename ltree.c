#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#include "libs/argLib.h"
#include "libs/stringEx.h"

// use to build : gcc -o ttree ltree.c libs/stringex.c libs/argLib.c

#define CNORM "\x1B[00m"
#define CBLACK "\x1B[7m"


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

bool findInFile (char *filename, char *item) {
	FILE *file;
	file = fopen(filename, "r");
	
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

int ls(char *dirname, DIR *directory, int indentation, int maxIndentation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType) {
	char *spacetab = " |  ";
	int dirsize = 0;

	struct dirent *item;
	directory = opendir(dirname);
	struct stat st;

	
	int currentFile = 0;
	int maxFiles = 2;
	struct dirent *files = calloc(maxFiles, sizeof(struct dirent));
	
	while ((item = readdir(directory)) != NULL) {
		if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {
			if (currentFile >= maxFiles) {
				int tempMaxFiles = maxFiles * 2;
				struct dirent *temp = realloc(files, tempMaxFiles * sizeof(struct dirent));
				if (temp != NULL) {
					files = temp;
					maxFiles = tempMaxFiles;
				}
			}

			files[currentFile] = *item;
			currentFile ++;
		}
	}


	// sorting folders
	for (int i = 0 ; i <= currentFile ; i++) {
		for (int j = i + 1 ; j <= currentFile ; j++) {
			if (files[j].d_name[0] < files[i].d_name[0] && files[j].d_type == files[i].d_type) {
				struct dirent tempFile = files[i];
				files[i] = files[j];
				files[j] = tempFile;
			} else if (strlen(files[j].d_name) < strlen(files[i].d_name) && files[j].d_name[0] == files[i].d_name[0]) {
				struct dirent tempFile = files[i];
				files[i] = files[j];
				files[j] = tempFile;
			}
		}
	}
	
	// recursive into folders 
	for (int i = 0 ; i <= currentFile ; i++) {
		if (files[i].d_type == DT_DIR) {
			// creating new path
			char temp = '/';
			char newpath[512];
			strcpy(newpath, dirname);
			strncat(newpath, &temp, 1);
			strcat(newpath, files[i].d_name);

			if (indentation + 1 < maxIndentation) {
				
				for (int i = 0 ; i < indentation ; i++) {
					printf("%s", spacetab);
				}

				// search filename
				if (strstr(files[i].d_name, search) && searchEnabled) {
					printf(CBLACK);
				}

				// print name and reset term color
				printf("%s \n", files[i].d_name);
				printf(CNORM);
			}


			dirsize += ls(newpath, directory, indentation + 1, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType);
			if (indentation + 1 < maxIndentation) {
				for (int i = 0 ; i < indentation ; i++) {
					printf("%s", spacetab);
				}

				printf(" |__");
				normalize(dirsize);
				printf("]\n");
			}
		}
	}

	// printing files 
	for (int i = 0 ; i <= currentFile ; i++) {
		if (files[i].d_type == DT_REG) {
			// creating new path
			char temp = '/';
			char newpath[512];
			strcpy(newpath, dirname);
			strncat(newpath, &temp, 1);
			strcat(newpath, files[i].d_name);

			// counting size
			stat(newpath, &st);
			dirsize += st.st_size;

			// printing
			if (indentation + 1 < maxIndentation) {
				// printing indentation
				for (int i = 0 ; i < indentation ; i++) {
					printf("%s", spacetab);
				}

				int tempLen = normalize(st.st_size);
				for (int i = 0 ; i < 10 - tempLen ; i++) {
					printf(" ");
				}
				printf("] ");
				
				// search filename
				if (strstr(files[i].d_name, search) && searchEnabled) {
					printf(CBLACK);
				}
				
				
				// file type
				stat(newpath, &st);
				char cFileType[64] = "";
				substring(files[i].d_name, cFileType, findChar(files[i].d_name, '.') + 1, 512);

				// search in files
				bool found = false;
				if (fileSearchEnabled && (strcmp(fileType, cFileType) == 0 || strcmp(fileType, "") == 0)) {
					found = findInFile(newpath, fileSearch);
				}
				if (found) {
					printf(CBLACK);
				}

				// print name and reset color
				printf("%s \n", files[i].d_name);
				printf(CNORM);

			}
		}
	}

	free(files);

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


	DIR *directory;
	
	printf(".\n");
	ls(".", directory, 0, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType);

	printf(CNORM);

	if (wait) {
		char tempC;
		scanf("%c", &tempC);
	}

	free(search);
	free(fileSearch);
	free(fileType);
	return 0;
}
