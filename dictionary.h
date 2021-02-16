#define GENERIC_ERROR "There was an error:\n"
#define FILE_NOT_OPEN_ERROR "Please open a file first\n"
#define COMMAND_NOT_FOUND_ERROR "Unknown command\n"
#define TOO_MANY_ARGUMENTS_ERROR "Too many arguments\n"
#define NOT_A_SEARCH_ERROR "Not a search\n"
#define NOT_FOUND_ERROR "Not found\n"

#define COMMAND_PROMPT "%d\t>", line_number
#define INSERT_MODE_PROMPT "%d\t:", line_number

#define LINE_NUMBER_INDICATOR "%d\t", line
#define CURRENT_LINE_INDICATOR "\x1b[33m>>\x1b[0m\t"
#define PAGING_INDICATOR "--MORE--"

#define HELP "h:\tHELP\ni:\tinsert mode\nd:\tdelete line\nn:\tenumerate lines\nq:\tquit\nN:\tgo to line N\n$:\tthe last line\nw:\tsave\n>:\tindent left\n<:\tindent right\n/:\tfind lines containing search\ns/a/b:\treplace the first match of a at the current line with b\n"
