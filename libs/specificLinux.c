
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <stdbool.h>
# include <dirent.h>
# include <sys/stat.h>
# include <errno.h>

# include "common.h"
# include "../stringEx/stringEx.h"

long long ls(char *dirname, DIR *directory, int indentation, int maxIndentation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType, bool displayAllFiles, bool use_old_style) {
	char *spacetab = " │  ";

	long long dirsize = 0;

	struct dirent *item;
	directory = opendir(dirname);
	if (directory == NULL) {
		fprintf(stderr, "Failed to open: %s, errno(%d)\n", dirname, errno);
		return 0;
	}
	struct stat st;

	bool displayFolderSize = (!searchEnabled && !fileSearchEnabled) || displayAllFiles;

	while ((item = readdir(directory)) != NULL) {
		if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {
			
			printf(CNORM);

			// make indentation
			if (indentation + 1 < maxIndentation) {
				printIndentation(indentation, use_old_style);
			}

			// checking if searched file name
			bool searchFound = false;
			if (strstr(item->d_name, search) && searchEnabled) {
				printf(CBLACK);

				searchFound = true;
				displayFolderSize = true;
			}

			// creating new path
			char temp = '/';
			char newpath[4096];
			strcpy(newpath, dirname);
			strncat(newpath, &temp, 1);
			strcat(newpath, item->d_name);
		
			/* int out = stat(newpath, &st); */
			int out = lstat(newpath, &st);
			if (out < 0) {
				fprintf(stderr, "%sStat failed! errno(%d), dir:%s%s\n", CRED, errno, newpath, CNORM);
				return 1;
			}

			// check if is folder
			if(item->d_type == DT_DIR) {
				if (indentation + 1 < maxIndentation || searchFound) {
					// fill indentation if hadn't already
					if (indentation + 1 >= maxIndentation) {
						printf("%s", CNORM);
						printIndentation(indentation, use_old_style);
						printf("%s", CBLACK);
					}

					if ((!searchEnabled && !fileSearchEnabled) || searchFound || displayAllFiles) printf("^ %s%s%s%s%s \t\t [ %s ]%s\n", CBOLD, CBLUE_FG, item->d_name, CNORM, CBOLD, newpath, CNORM);
				}

				bool hasPermission = (st.st_mode & S_IRUSR) | (st.st_mode & S_IRGRP);
				// recursive call
				if (hasPermission) dirsize += ls(newpath, directory, indentation + 1, maxIndentation, search, searchEnabled, fileSearch, fileSearchEnabled, fileType, displayAllFiles, use_old_style);

			} else {
				// file type
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
					displayFolderSize = found || (!searchEnabled && !fileSearchEnabled) || displayAllFiles || displayFolderSize;
				}
				if (found) {
					printf(CBLACK);
				}
				// print name
				if (indentation + 1 < maxIndentation || found || searchFound) {

					if ((!searchEnabled && !fileSearchEnabled) || found || searchFound || found || displayAllFiles) {
						// fill indentation if hadn't already
						if (indentation + 1 >= maxIndentation) {
							printf("%s", CNORM);
							printIndentation(indentation, use_old_style);
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
			}
			printf(CNORM);
		}
	}
	
	if ((indentation < maxIndentation && displayFolderSize) || indentation <= 0) {
		if (indentation <= 0) printf(" >v---%sTOTAL:%s----------------------< \n", CBOLD, CNORM);

		for (int i = 0 ; i < indentation - 1 ; i++) {
			printf("  %s ", use_old_style ? PIPE_OLD : PIPE);
		}
		printf("  %s%s", use_old_style ? LEG_OLD : LEG, use_old_style ? DASH_OLD : DASH);
		
		int tempLen = normalize(dirsize);
		for (int i = 0 ; i < 12 - tempLen ; i++) {
			printf(" ");
		}
		printf("]");
		printf("\n");

		// spacer after a directory
		for (int i = 0 ; i < indentation - 1 ; i++) {
			printf("  %s ", use_old_style ? PIPE_OLD : PIPE);
		}
		printf("\n");
	}

	closedir(directory);

	return dirsize;

}
