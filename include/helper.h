#ifndef HELPER_FUNC_H_
#define HELPER_FUNC_H_

/* Checks for a word separator */
int is_separator(char);

/* Read a character without echoing to the screen */
int getchar_silent();

/* Case-sensitive string concatenation */
char *strcat_casesens(char *, const char *);

#endif
