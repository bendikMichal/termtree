
# include <stdio.h>
# include <math.h>
# include <string.h>
# include <stdbool.h>

# include "common.h"

char LLabels[][MAX_LABEL_LEN] = {
	"--help",
	"--search-in-file",
	"--find",
	"--max-index",
	"--leave-open",
	"--version",
	"--all",
	"--use-old-style"
};

char SLabels[][MAX_LABEL_LEN] = {
	"-h",
	"-s",
	"-f",
	"-i",
	"-L",
	"-v",
	"-a",
	"-o"
};

char *rawHelpString = 
	"Help for TermTree \n"
	"Usage: ttree [ path ](optional) [ args ] ...\n"
	"\t %s, %s \n\t\t- shows help\n"
	"\t %s, %s <text>/<file_type> \n\t\t- search in file, if <file_type> is empty, search will be in all files\n"
	"\t %s, %s <file/folder name> \n\t\t- find a specific file/folder name\n"
	"\t %s. %s <number> \n\t\t- replace <number> with a whole number specifing how deep into the folders should the seach go\n"
	"\t %s, %s \n\t\t- wait for a keypress to close the program\n"
	"\t %s, %s \n\t\t- display version number\n"
	"\t %s, %s \n\t\t- display all files otherwise hidden by search\n"
	"\t %s, %s \n\t\t- use old visual style without the pretty glyphs\n";

int normalize (long long bytes) {
	if (bytes < 0) return 0;
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
		sprintf(buf, "[ %lld b  ", bytes);
		printf("%s", buf);
		return strlen(buf);
	} 
	return 0;
}

bool findInFile (char *filename, char *item) {
	FILE *file;
	file = fopen(filename, "r");
	
	if (file == NULL) {
		fprintf(stderr, "Failed to open file for reading! filename: %s\n", filename);
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

void printIndentation (int indentation, bool use_old_style) {
	for (int i = 0 ; i < indentation ; i++) {
		if (!use_old_style) {
			if (i < indentation - 1) printf("  %s ", PIPE);
			else printf("  %s%s ", LEAD, DASH);

		} else {
			printf("  %s ", PIPE_OLD);

		}
	}
}

