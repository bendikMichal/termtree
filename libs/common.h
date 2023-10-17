
# ifndef __COMMON_H
# define __COMMON_H

# include "../tree.h"

extern char LLabels[][MAX_LABEL_LEN];
extern char SLabels[][MAX_LABEL_LEN];
extern char *rawHelpString;

int normalize(long long bytes);
bool findInFile (char *filename, char *item);

# endif
