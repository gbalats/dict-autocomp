#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"


/* Helper function that allocates an internal node */
static trie_node_t *trie_alloc_node(trie_node_t *parent)
{
    trie_node_t *node = malloc(sizeof(trie_node_t));

    node->parent = parent;
    node->word = NULL;
    node->label = 0;

    for (int i = 0; i < MAX_CHILDREN; i++)
        node->children[i] = NULL;

    for (int i = 0; i < N; i++)
        node->completions[i] = NULL;

    return node;
}

/* Helper function to update common word completions */
static void trie_update(trie_leaf_t *leaf)
{
    int freq = ++leaf->frequency;

    for (trie_node_t *node = leaf->parent; node; node = node->parent)
    {
        int pos = N - 1;

        /* Advance empty slots */
        while (pos > 0 && !node->completions[pos])
            pos--;

        /* No completions */
        if (!node->completions[0]) {
            node->completions[0] = leaf;
            continue;
        }

        /* Sanity check */
        assert(node->completions[pos]);

        /* Stop if no more updating is needed */
        if (pos == N - 1 && node->completions[pos]->frequency > freq)
            break;

        /* New position in existing completions */
        int newpos = 0;

        /* Search if completion exists and bring to front */
        for (int i = pos; i >= 0; i--)
        {
            trie_leaf_t *other = node->completions[i];

            /* Skip less frequent completions */
            if (other->frequency <= freq && other != leaf)
                continue;

            /* Not present; found position to insert */
            if (other != leaf)
            {
                newpos = i + 1;
                break;
            }

            assert(other == leaf);

            /* cannot push completion to front; so stop */
            if (i <= 0 || node->completions[i - 1]->frequency > freq)
                goto INSERTED_CONTINUE;

            /* Swap with previous completion */
            node->completions[i] = node->completions[i - 1];
            node->completions[i - 1] = leaf;
        }

        /* Move last completion if possible */
        if (pos + 1 < N)
            node->completions[pos + 1] = node->completions[pos];

        /* Shift the rest */
        for (int i = pos; i > newpos; i--)
            node->completions[i] = node->completions[i - 1];

        /* Insert new completion */
        node->completions[newpos] = leaf;

    INSERTED_CONTINUE:
        ;
    }
}



/* Get word from leaf */
int trie_getword(trie_leaf_t *leaf, char *buffer)
{
    char *start = buffer;

    /* Append labels */
    for (trie_node_t *node = leaf->parent; node->parent; node = node->parent)
        *buffer++ = node->label;

    *buffer = '\0';
    int len = buffer - start;
    char *end = buffer - 1;

    /* Reverse in place */
    while (start < end)
    {
        char tmp = *start;
        *start++ = *end;
        *end--   = tmp;
    }

    return len;
}

/* Initialize trie */
void trie_init(trie_node_t **root)
{
    *root = trie_alloc_node(NULL);
}

/* De-initialize trie */
void trie_destroy(trie_node_t *root)
{
    if (!root)
        return;

    for (int i = 0; i < MAX_CHILDREN; i++)
        trie_destroy(root->children[i]);

    free(root->word);
    free(root);
}


/* Add new word to trie */
void trie_add(trie_node_t *root, const char *word)
{
    for (const char *p = word; *p; p++)
        assert(isalpha(*p));

    trie_node_t *node = root;

    while(*word) {
        int index = *word++ - 'a';
        assert(0 <= index && index < MAX_CHILDREN);

        trie_node_t **next = &node->children[index];

        if (! *next) {
            *next = trie_alloc_node(node);
            (*next)->label = index + 'a';
        }

        node = *next;
    }

    assert(node);

    if (!node->word)
    {
        node->word = malloc(sizeof(trie_leaf_t));
        node->word->parent = node;
        node->word->frequency = 0;
    }

    trie_update(node->word);
}

/* Get word */
char **trie_getwords(trie_node_t *root, const char *prefix)
{
    static char *words[N];
    static char suffixes[BUFSIZ];

    trie_node_t *node = root;

    /* Lookup prefix */
    while(*prefix) {
        int index = *prefix++ - 'a';
        assert(0 <= index && index < MAX_CHILDREN);

        node = node->children[index];

        if (!node)
            return NULL;
    }

    /* Pointer to free space */
    char *buff = suffixes;

    /* Get each word */
    for (int i = 0; i < N; i++)
    {
        /* No completion */
        if (!node->completions[i])
        {
            words[i] = NULL;
            continue;
        }

        /* Append next completion */
        int len = trie_getword(node->completions[i], buff);

        /* Terminate word */
        words[i] = buff;
        buff += len + 1;
    }

    return words;
}

/* Load trie from dictionary file */
void trie_load(trie_node_t *root, const char *path)
{
    FILE *dictionary;

    /* Open dictionary file */
    if ((dictionary = fopen(path, "r")) == NULL) {
        perror("Failed to open dictionary");
        exit(EXIT_FAILURE);
    }

    static char word[BUFSIZ];

    /* Read each word */
    while (fgets(word, BUFSIZ, dictionary) != NULL) {
        int pos = strlen(word) - 1;

        if (word[pos] == '\n')
            word[pos] = '\0';

        trie_add(root, word);
    }

    fclose(dictionary);
}

/* Store trie to dictionary file */
void trie_store(trie_node_t *root, const char *path)
{
    FILE *dictionary;

    /* Open dictionary file */
    if ((dictionary = fopen(path, "w")) == NULL) {
        perror("Failed to open dictionary");
        exit(EXIT_FAILURE);
    }

    trie_display(root, dictionary);
    fclose(dictionary);
}


/* Display trie */
void trie_display(trie_node_t *root, FILE *stream)
{
    if (!root)
        return;

    if (root->word)
    {
        static char buf[BUFSIZ];

        trie_getword(root->word, buf);
        fprintf(stream, "%s\n", buf);
    }

    for (int i = 0; i < MAX_CHILDREN; i++)
        trie_display(root->children[i], stream);
}

void trie_checkfreq(trie_node_t *root)
{
    if (!root)
        return;

    for (int i = 0; i < MAX_CHILDREN; i++)
        trie_checkfreq(root->children[i]);

    int prevfreq = INT_MAX;

    for (int i = 0; i < N; i++)
    {
        int freq = root->completions[i]
            ? root->completions[i]->frequency
            : -1;

        assert(freq <= prevfreq);
        prevfreq = freq;
    }
}
