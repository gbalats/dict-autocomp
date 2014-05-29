#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "term.h"
#include "trie.h"

trie_node_t *root;

static void usage(const char *progname)
{
    /* Print error message */
    fprintf(stderr,
            "Usage: %s [-d PATH]"
            ""
            "    -d PATH     specify a different dictionary to be used",
            progname);

    /* Exit while indicating failure */
    exit(EXIT_FAILURE);
}

static void clear()
{
    trie_destroy(root);
}

int main (int argc, const char *argv[])
{
    char path[256];

    /* Determine path to dictionary */

    if (argc == 3)              /* Path given explicitly */
    {
        if (strcmp(argv[1], "-d"))
            usage(argv[0]);

        strcpy(path, argv[2]);
    }
    else if (argc == 1)         /* Default path */
    {
        const char *default_base = "/.dict";
        const char *home_dir = getenv("HOME");

        strcpy(path, home_dir);
        strcat(path, default_base);
    }
    else {
        usage(argv[0]);
    }

    /* Load dictionary into memory */

    printf("Reading dictionary %s ...\n", path);

    trie_init(&root);
    atexit(clear);
    trie_load(root, path);

    /* Start editing loop */
    interact();

    return EXIT_SUCCESS;
}
