#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdio.h>

typedef enum {ACTIVE_COMPL, INACTIVE_COMPL} state;

typedef struct Console
{
    char linebuf_wo[BUFSIZ];    /* Current line buffer (with offset) */
    char *linebuf;              /* Current line buffer */
    state current_state;        /* Current completion state */
    char **completions;         /* Active completions */
    char prefix[128];           /* Completion prefix */

} Console;


void  console_init(Console *);
char *console_prevtoken(Console *, int);
void  console_display_compline(Console *, FILE *);
int   console_autocomplete(Console *, int);
void  console_append(Console *, char);
void  console_erase(Console *);
void  console_flush(Console *);

#endif
