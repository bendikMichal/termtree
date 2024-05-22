
# ifndef __TREEH
# define __TREEH

# define CNORM "\x1B[00m"
# define CBOLD "\x1B[01m"
# define CBLACK "\x1B[7m"
# define CRED "\x1B[41m"
# define CRED_FG "\x1B[31m"
# define CGREEN_FG "\x1B[32m"
# define CBLUE_FG "\x1B[34m"
# define MAX_LABEL_LEN 128

# define DASH "─"
# define PIPE "│"
# define LEAD "├"
# define LEG  "└"

# define DASH_OLD "_"
# define PIPE_OLD "|"
# define LEG_OLD  "|"

# define version "2.3.0 rel2"

enum ARG_TYPES {
	HELP,
	SEARCH_IN_FILE,
	FIND,
	MAX_INDEX,
	LEAVE_OPEN,
	VERSION,
	ALL,
	OLD_STYLE
};

# endif
