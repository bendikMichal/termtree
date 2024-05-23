
# ifndef __ARGLIBH
# define __ARGLIBH

# include "../tree.h"
# define DEFAULT_ARG_START 1

typedef struct SEARCH_POINTERS {
	char *fileSearch;
	char *fileType;
} SEARCH_POINTERS;

char* setSearch(char *argv, char *search);

int setIndentation(char *argv);

int setFileSearch(char *argv, SEARCH_POINTERS* sp);

typedef struct ARG {
	char *label;
	char *value;
	char al[MAX_LABEL_LEN];
	char av[2048];
} ARG;

ARG* getArgs(int argc, char *argv[], int arg_start);

int getLabelCount(int argc, char *argv[], int arg_start);

# endif
