#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "console.h"
#include "helper.h"
#include "trie.h"

#define VT100
#define ASCII_ESC 27

extern trie_node_t *root;

/* Initialize console */
void console_init(Console *console)
{
    console->linebuf = console->linebuf_wo + 1;
    console->linebuf[-1] = '\0';
    console->current_state = INACTIVE_COMPL;
    console->completions = NULL;
}

/* Return console's last token */
char *console_prevtoken(Console *console, int skip)
{
    char *line = console->linebuf;
    size_t nChars = strlen(line) - skip;

    char *ptr = &line[nChars];
    char last = line[nChars];

    /* Go backwards while skipping letters */
    for (*ptr-- = '\0'; isalpha(*ptr); ptr--)
        ;

    /* Sanity checks */
    assert(!isalpha(*ptr));
    ptr++;
    assert(!*ptr || isalpha(*ptr));

    /* Copy token */
    char *copy = strdup(ptr);

    /* Restore last character */
    line[nChars] = last;

    /* Make lower-case */
    for (char *p = copy; *p; ++p)
        *p = tolower(*p);

    return copy;
}

size_t console_addsuffix(Console *console, const char *word)
{
    const size_t nChars = strlen(console->linebuf);

    /* Erase prefix from line buffer */
    const size_t prefixlen = strlen(console->prefix);
    char *ptr = &console->linebuf[nChars - prefixlen];

    /* Sanity checks */
    for (int i = 0; i < prefixlen; i++)
        assert(tolower(ptr[i]) == word[i]);

    /* Append completion to line buffer */
    strcat_casesens(ptr, word);
    strcat(ptr, " ");
    trie_add(root, word);

    /* Compute new size */
    size_t size = strlen(console->linebuf);

    /* More sanity checks */
    const size_t complen = strlen(word);
    assert(size == nChars - prefixlen + complen + 1);

    return size;
}

/* Display completion line to stream */
void console_display_compline(Console *console, FILE *stream)
{
    char *prefix = console_prevtoken(console, 0);
    char **words = trie_getwords(root, prefix);

    /* No completions */
    if (!words || !words[0])
        return;

    /* Cache prefix */
    strcpy(console->prefix, prefix);

    /* One single completion */
    if (!words[1])
    {
        /* Append suffix */
        size_t nChars = console_addsuffix(console, words[0]);
        console->current_state = INACTIVE_COMPL;

        /* Compute  */
        size_t offset = 1 + strlen(words[0]) - strlen(prefix);

        /* Show output to screen */
        printf("%s", console->linebuf + nChars - offset);
        return;
    }

    /* Prefix */
    fprintf(stream, "\n>[%s] ", prefix);

    /* Append each completion */
    for (int i = 0; i < N; i++)
        if (words[i])
            fprintf(stream, "%s ", words[i]);

    /* Suffix */
    fputc('\n', stream);

    /* Renew console state */
    console->current_state = ACTIVE_COMPL;
    console->completions = words;
    free(prefix);
}

void console_flush(Console *console)
{
    if (console->current_state != ACTIVE_COMPL)
        return;

#ifdef VT100
    /* Move one line up */
    printf("%c[1A", ASCII_ESC);

    /* Clear completions */
    printf("%c[2K", ASCII_ESC);

    /* Go to the beginning of the previous line */
    printf("%c[1A\r", ASCII_ESC);
#endif

    printf("%s", console->linebuf);
    console->current_state = INACTIVE_COMPL;
}

/* Auto-complete word */
int console_autocomplete(Console *console, int choice)
{
    if (console->current_state != ACTIVE_COMPL)
        return -1;

    if (!console->completions || choice >= N)
        return -1;

    char *completion = console->completions[choice];

    if (!completion)
        return -1;

    /* Append suffix */
    console_addsuffix(console, completion);

    /* Flush screen */
    console_flush(console);

    assert(console->current_state == INACTIVE_COMPL);

    return 0;
}


/* Append character */
void console_append(Console *console, char c)
{
    char *line = console->linebuf;
    size_t nChars = strlen(line);

    putchar(c);
    line[nChars] = c;
    line[nChars + 1] = '\0';

    /* Sanity check */
    assert(strlen(line) == nChars + 1);

    /* Store previous word */
    if (is_separator(c))
    {
        char *word = console_prevtoken(console, 1);

        /* Add non-empty word to trie */
        if (*word)
            trie_add(root, word);

        free(word);

        trie_checkfreq(root);   /* for debugging */
    }

    /* Empty line */
    if (c == '\n')
        line[0] = '\0';

    /* Sanity check */
    assert(c != '\n' || strlen(line) == 0);

    /* Set state */
    console->current_state = INACTIVE_COMPL;
}

/* Erase one character */
void console_erase(Console *console)
{
    char *line = console->linebuf;
    size_t nChars = strlen(line);

    /* New line - do nothing */
    if (!line[0])
        return;

    /* Move the cursor one step back, print space, and move
     * cursor back once more to imitate backward character
     * deletion.
     */

    printf("\b \b");
    line[nChars - 1] = '\0';

    /* Sanity check */
    assert(strlen(line) == nChars - 1);

    /* Set state */
    console->current_state = INACTIVE_COMPL;
}
