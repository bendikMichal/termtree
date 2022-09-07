#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

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

void ls(char *dirname, DIR *directory, int intedation) {
	struct dirent *item;
	directory = opendir(dirname);
	struct stat st;
	
	if (directory) {
		while ((item = readdir(directory)) != NULL) {
			if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {
				// make intedation
				for (int i = 0 ; i < intedation ; i++) {
					printf("  | ");
				}

				// creating new path
				char temp = '/';
				char newpath[512];
				strcpy(newpath, dirname);
				strncat(newpath, &temp, 1);
				strcat(newpath, item->d_name);

				// check if is folder
				stat(newpath, &st);
			

				if(S_ISDIR(st.st_mode)) {
					printf("^ %s\n", item->d_name);
					// recursive call
					ls(newpath, directory, intedation + 1);
				} else {
					// print name
					normalize(st.st_size);
					printf("%s\n", item->d_name);
				}
				
			}
		}
		closedir(directory);
	}
}


int main () {
	DIR *directory;
	
	printf(".\n");
	ls(".", directory, 0);

	return 0;
}
