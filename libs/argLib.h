
# ifndef __ARGLIBH
# define __ARGLIBH

# include "../tree.h"

void setSearch(char *argv, char *search);

int setIndentation(char *argv);

int setFileSearch(char *argv, char *fileSearch, char *fileType);

typedef struct ARG {
	char *label;
	char *value;
	char al[MAX_LABEL_LEN];
	char av[2048];
} ARG;

ARG* getArgs(int argc, char *argv[]);

int getLabelCount(int argc, char *argv[]);

# endif
