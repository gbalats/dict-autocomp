#ifndef TERM_FUNC_H_
#define TERM_FUNC_H_

#define LINESIZE 128
#define BACKSPACE 0x7f

#include "trie.h"

extern trie_node_t *root;

int getchar_silent();
void interact();

#endif
