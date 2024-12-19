#ifndef INDEX_H
#define INDEX_H

typedef struct TrieNode {
    int count;
    struct TrieNode *children[26];
} TrieNode;

TrieNode *indexPage(const char *url, int *totalNumTerms);
int addWordOccurrence(TrieNode *root, const char *words);
void printTrieContents(TrieNode *root, char *buffer, int charCount);
int freeTrieMemory(TrieNode *node);
int parseBuffer(char *buffer, char **words);
int getText(const char *srcAddr, char *buffer, const int bufSize);

#endif
