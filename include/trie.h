#ifndef TRIE_IMPL_H_
#define TRIE_IMPL_H_

#define N 5
#define MAX_CHILDREN 26

#include <stdlib.h>


/* Forward declarations */

typedef struct trie_node_t trie_node_t;
typedef struct trie_leaf_t trie_leaf_t;


/* Trie inner node definition */

struct trie_node_t
{
    trie_node_t *parent;                 /* Parent pointer */
    trie_node_t *children[MAX_CHILDREN]; /* Children nodes */
    trie_leaf_t *completions[N];         /* Fast-search leaf nodes */
    trie_leaf_t *word;                   /* To leaf node */
    char label;
};


/* Trie leaf node definition */

struct trie_leaf_t
{
    trie_node_t *parent;                 /* Parent pointer */
    size_t frequency;                    /* Number of occurrences */
};


/* Trie functions */

void   trie_init(trie_node_t **root);
void   trie_destroy(trie_node_t *root);
void   trie_load(trie_node_t *root, const char *path);
void   trie_store(trie_node_t *root, const char *path);
char **trie_getwords(trie_node_t *root, const char *prefix);
int    trie_getword(trie_leaf_t *leaf, char *buffer);
void   trie_display(trie_node_t *root, FILE *stream);
void   trie_add(trie_node_t *root, const char *word);
void   trie_checkfreq(trie_node_t *root);

#endif
