#include <ctype.h>
#include <stdio.h>
#include <termios.h>

#include "console.h"
#include "helper.h"
#include "term.h"


void interact()
{
    Console console;
    int next;

    /* Initialize console */
    console_init(&console);

    /* Keep reading one character at a time */
    while ((next = getchar_silent()) != EOF)
    {
        /* Print normal characters */
        if (isalpha(next) || is_separator(next))
        {
            console_flush(&console);
            console_append(&console, (char) next);
            continue;
        }

        /* Digits */
        if (isdigit(next))
            console_autocomplete(&console, next - '1');

        /* Treat special characters differently */
        switch(next) {

        case '\t':              /* Just read a tab */

            console_display_compline(&console, stderr);
            break;

        case BACKSPACE:         /* Just read a backspace */

            console_erase(&console);
            break;

        case VEOF:              /* Just read EOF (Ctrl-D) */

            /* The EOF character is recognized only in canonical input
             * mode. In noncanonical input mode, all input control
             * characters such as Ctrl-D are passed to the application
             * program exactly as typed. Thus, Ctrl-D now produces
             * this character instead.
             */

            printf("\n\nExiting. Bye...");
            goto THE_END;
        default:
            continue;
        }
    }
THE_END:
    putchar('\n');
}
