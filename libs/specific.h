
# ifndef __SPECIFIC_H
# define __SPECIFIC_H

# include <dirent.h>

long long ls(char *dirname, DIR *directory, int indentation, int maxIndentation, char *search, bool searchEnabled, char *fileSearch, bool fileSearchEnabled, char *fileType, bool displayAllFiles, bool use_old_style);

# endif
