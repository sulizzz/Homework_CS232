#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "index.h"

TrieNode *indexPage(const char *url, int *totalNumTerms) {
    *totalNumTerms = 0;

    TrieNode *root = malloc(sizeof(TrieNode));
    if (root == NULL) {
        printf("Failed memory allocation for TrieNode root\n");
        return NULL;
    }

    root->count = 0;
    for (int i = 0; i < 26; i++) {
        root->children[i] = NULL;
    }

    char *buffer = malloc(sizeof(char) * 30000);
    if (buffer == NULL) {
        printf("Failed memory allocation for char* buffer\n");
        free(root);
        return NULL;
    }

    int bytesRead = getText(url, buffer, 30000);
    if (bytesRead <= 0) {
        printf("This URL was not grabbed: %s\n", url);
        free(buffer);
        free(root);
        return NULL;
    }

    char **words = malloc(sizeof(char *) * 30000);
    if (words == NULL) {
        fprintf(stderr, "Memory allocation for words array failed\n");
        free(buffer);
        free(root);
        return NULL;
    }

    int wordCount = parseBuffer(buffer, words);

    for (int i = 0; i < wordCount; i++) {
        addWordOccurrence(root, words[i]);
        (*totalNumTerms)++;
        free(words[i]);
    }

    free(words);
    free(buffer);

    return root;
}

int addWordOccurrence(TrieNode *root, const char *words) {
    TrieNode *currentNode = root;

    for (int i = 0; words[i] != '\0'; i++) {
        char ch = tolower(words[i]);

        if (ch < 'a' || ch > 'z') {
            fprintf(stderr, "Error: INVALID CHARACTER '%c' in word \"%s\"\n", ch, words);
            continue;
        }

        int index = ch - 'a';

        if (currentNode->children[index] == NULL) {
            currentNode->children[index] = malloc(sizeof(TrieNode));
            if (currentNode->children[index] == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for TrieNode\n");
                return -1;
            }
            currentNode->children[index]->count = 0;
            for (int j = 0; j < 26; j++) {
                currentNode->children[index]->children[j] = NULL;
            }
        }
        currentNode = currentNode->children[index];
    }
    currentNode->count++;
    return 0;
}

void printTrieContents(TrieNode *root, char *buffer, int charCount) {
    if (root == NULL) {
        return;
    }

    if (root->count > 0) {
        buffer[charCount] = '\0';
        for (int i = 0; i < root->count; i++) {
            printf("\t%s\n", buffer);
        }
    }

    for (int i = 0; i < 26; i++) {
        if (root->children[i] != NULL) {
            buffer[charCount] = 'a' + i;
            printTrieContents(root->children[i], buffer, charCount + 1);
        }
    }
}

int freeTrieMemory(TrieNode *node) {
    if (node == NULL) {
        return 0;
    }
    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            freeTrieMemory(node->children[i]);
        }
    }
    free(node);
    return 0;
}

int getText(const char *srcAddr, char *buffer, const int bufSize) {
    FILE *pipe;
    int bytesRead;

    snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

    pipe = popen(buffer, "r");
    if (pipe == NULL) {
        fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
                buffer);
        return 0;
    }

    bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
    buffer[bytesRead] = '\0';

    pclose(pipe);

    return bytesRead;
}

int parseBuffer(char *buffer, char **words) {
    int count = 0;
    int wordCount = 0;

    while (buffer[count] != '\0') {
        if (buffer[count] == 'b' && buffer[count + 1] == '\'') {
            count += 2;
            continue;
        }

        if (isalpha(buffer[count])) {
            int charCount = 0;

            while (isalpha(buffer[count])) {
                charCount++;
                count++;
            }

            words[wordCount] = malloc(sizeof(char) * (charCount + 1));

            if (words[wordCount] == NULL) {
                fprintf(stderr, "Memory allocation failed for word %d.\n", wordCount);
                return wordCount;
            }

            for (int i = 0; i < charCount; i++) {
                words[wordCount][i] = tolower(buffer[count - charCount + i]);
            }

            words[wordCount][charCount] = '\0';
            wordCount++;
        }

        count++;
    }

    return wordCount;
}
