#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crawler.h"

int contains(const struct listNode *pNode, const char *addr) {
    if (pNode == NULL) {
        return 0;
    }

    if (strcmp(pNode->addr, addr) == 0) {
        return 1;
    } else {
        return contains(pNode->next, addr);
    }
}

void insertBack(struct listNode *pNode, const char *addr) {
    struct listNode *newNode = (struct listNode *)malloc(sizeof(struct listNode));
    if (newNode == NULL) {
        printf("Could not allocate memory on heap in insertBack function");
        return;
    }

    strncpy(newNode->addr, addr, MAX_ADDR_LENGTH);
    newNode->next = NULL;

    if (pNode->next == NULL) {
        pNode->next = newNode;
        return;
    } else {
        insertBack(pNode->next, addr);
    }
}

void printAddresses(const struct listNode *pNode) {
    if (pNode != NULL) {
        printf("%s\n", pNode->addr);
        printAddresses(pNode->next);
    }
}

void destroyList(struct listNode *pNode) {
    if (pNode == NULL) {
        return;
    }

    destroyList(pNode->next);
    free(pNode);
}

int getLink(const char *srcAddr, char *link, const int maxLinkLength) {
    const int bufSize = 1000;
    char buffer[bufSize];

    int numLinks = 0;

    FILE *pipe;

    snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getLinks.py", srcAddr);

    pipe = popen(buffer, "r");
    if (pipe == NULL) {
        fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
        return 0;
    }

    fscanf(pipe, "%d\n", &numLinks);

    if (numLinks > 0) {
        int linkNum;
        double r = (double)rand() / ((double)RAND_MAX + 1.0);

        for (linkNum = 0; linkNum < numLinks; linkNum++) {
            fgets(buffer, bufSize, pipe);

            if (r < (linkNum + 1.0) / numLinks) {
                break;
            }
        }

        strncpy(link, buffer, maxLinkLength);
        link[maxLinkLength - 1] = '\0';

        char *pNewline = strchr(link, '\n');
        if (pNewline != NULL) {
            *pNewline = '\0';
        }
    }

    pclose(pipe);

    return (numLinks > 0) ? 1 : 0;
}

void crawlURL(char *url, int numHops, int *totalIndexed, int maxPages, TrieNode **trieList) {
    if (*totalIndexed >= maxPages) {
        return;
    }

    char startAddr[MAX_ADDR_LENGTH];
    strncpy(startAddr, url, MAX_ADDR_LENGTH);

    struct listNode *pListStart = malloc(sizeof(struct listNode));
    if (pListStart == NULL) {
        printf("Error: Memory allocation for list node failed in crawlURL function.");
        return;
    }

    int totalNumTerms = 0;
    TrieNode *root = indexPage(url, &totalNumTerms);

    if (root == NULL) {
        fprintf(stderr, "Error: Failed to index page: %s\n", url);
        free(pListStart);  // Fix memory leak
        return;
    }

    trieList[*totalIndexed] = root;

    printf("Indexing... \n%s\n", url);
    char buffer[30000];
    printTrieContents(root, buffer, 0);
    printf("\n");

    strncpy(pListStart->addr, startAddr, MAX_ADDR_LENGTH);
    pListStart->next = NULL;

    for (int hopNum = 1; hopNum <= numHops && *totalIndexed < maxPages; hopNum++) {
        char destAddr[MAX_ADDR_LENGTH];
        int res = getLink(startAddr, destAddr, MAX_ADDR_LENGTH);

        if (!res) {
            printf("Dead end on hop %d: no outgoing links\n", hopNum);
            break;
        }

        if (contains(pListStart, destAddr)) {
            printf("Cycle detected on hop %d: address %s\n", hopNum, destAddr);
            continue;
        }

        insertBack(pListStart, destAddr);
        strncpy(startAddr, destAddr, MAX_ADDR_LENGTH);
    }

    (*totalIndexed)++;
    destroyList(pListStart);
}
