#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "helper.h"

typedef enum {LOWERCASE, ALLCAPS, CAPS} Case;

int is_separator(char c)
{
    return ispunct(c) || c == ' ' || c == '\n';
}

char *strcat_casesens(char *prefix, const char *completion)
{
    /* Compute case */
    Case sens = !prefix[0] || islower(prefix[0])
        ? LOWERCASE : (islower(prefix[1]) ? CAPS : ALLCAPS);

    /* Skip prefix */
    size_t plen = strlen(prefix);
    const char *suffix = completion + plen;
    char *ptr = prefix + plen;

    /* Append suffix as it is */
    strcat(prefix, suffix);

    switch (sens) {
    case ALLCAPS:
        /* Make upper-case */
        for (char *p = ptr; *p; ++p)
            *p = toupper(*p);
        break;
    case CAPS:
    case LOWERCASE:
        ;
    }

    return ptr;
}

int getchar_silent()
{
    int ch;
    struct termios oldt, newt;

    /* Retrieve old terminal settings */
    tcgetattr(STDIN_FILENO, &oldt);

    /* Disable canonical input mode, and input character echoing. */
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* Read next character, and then switch to old terminal settings. */
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
