#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crawler.h"
#include "index.h"

int main(int argc, char **argv) {
    char *fName = argv[1];
    char fullFName[1000] = "webSearchSol/urlFiles/";
    int MAX_N = atoi(argv[2]);
    char **urlList = malloc(MAX_N * sizeof(char *));
    TrieNode **trieList = malloc(MAX_N * sizeof(TrieNode *));

    if (urlList == NULL || trieList == NULL) {
        printf("Failed to allocate memory for urlList");
        return 2;
    }

    long seed = atol(argv[3]);
    srand(seed);

    // Making sure the input is in valid file path format
    if (strncmp(fName, "webSearchSol/urlFiles/", strlen("webSearchSol/urlFiles/")) == 0) {
        strncpy(fullFName, fName, strlen(fName));
    }
    else if (fName[0] == '.' && fName[1] == '/') {
        strncpy(fullFName, fName + 2, strlen(fName) - 1);
    }
    else {
    strcat(fullFName, fName);
    }

    FILE *fp = fopen(fullFName, "r");

    if (fp == NULL) {
        printf("No File Found.\n");
        return 1;
    }

    char line[100];
    int count = 0;

    while (fgets(line, 100, fp) != NULL && count < MAX_N) {
        char *space = strchr(line, ' ');
        char url[MAX_ADDR_LENGTH];
        int numHops;

        if (space != NULL) {
            int finalIndex = space - line;
            strncpy(url, line, finalIndex);
            url[finalIndex] = '\0';
            numHops = atoi(space + 1);

            urlList[count] = strdup(url);
            crawlURL(url, numHops, &count, MAX_N, trieList);
        }
    }

    for (int i = 0; i < count; i++) {
        if (urlList[i] != NULL) {
            free(urlList[i]);
        }
    }

    free(urlList);
    free(trieList);
    fclose(fp);

    return 0;
}
