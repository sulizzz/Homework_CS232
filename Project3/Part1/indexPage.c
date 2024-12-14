/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_ADDR_LENGTH 1000

typedef struct TrieNode
{
  int count;
  struct TrieNode *children[26];
  // for the alphabet of 26 letters
} TrieNode;

/*
 * a node in our linked-list of web addresses
 */
struct listNode
{
  char addr[MAX_ADDR_LENGTH];

  struct listNode *next;
};
/*
 * returns 1 if the list starting at pNode contains the address addr,
 *    and returns 0 otherwise
 */
int contains(const struct listNode *pNode, const char *addr);

/*
 * inserts the address addr as a new listNode at the end of
 *    the list
 */
void insertBack(struct listNode *pNode, const char *addr);

/*
 * prints the addresses from pNode to the end of the list,
 *   one on each line
 */
void printAddresses(const struct listNode *pNode);

/*
 * frees the memory associated with this node and all subsequent nodes
 */
void destroyList(struct listNode *pNode);

/*
 * srcAddr should be a web address (e.g., http://www.yahoo.com).
 * link should point to an array of maxLinkLength characters.
 * getLink returns 1 if a link was found and 0 otherwise.
 * If a link was found, "link" will be filled in with the web address.
 */
int getLink(const char *srcAddr, char *link, const int maxLinkLength);

/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

/* TODO: change this return type */
TrieNode *indexPage(const char *url, int *totalNumTerms);

int addWordOccurrence(TrieNode *root, const char *words);

void printTrieContents(/* TODO: any parameters you need */);

int freeTrieMemory(/* TODO: any parameters you need */);

int getText(const char *srcAddr, char *buffer, const int bufSize);

int parseBuffer(char *buffer, char **words);

void crawlURL(char *url, int numHops, int *totalIndexed, int maxPages, TrieNode **trieList);

int main(int argc, char **argv)
{
  char *fName = argv[1];
  char fullFName[50] = "webSearchSol/urlFiles/";
  int MAX_N = atoi(argv[2]);
  char **urlList = malloc(MAX_N * sizeof(char *));
  TrieNode **trieList = malloc(MAX_N * sizeof(TrieNode *));

  if (urlList == NULL || trieList == NULL)
  {
    printf("Failed to allocate memory for urlLise");
    return 2;
  }

  long seed = atol(argv[3]);
  srand(seed);

  strcat(fullFName, fName);

  FILE *fp = fopen(fullFName, "r");

  // Check for file and memory allocation of fp and urlList
  if (fp == NULL)
  {
    printf("No File Found.");
    return 1;
  }

  if (urlList == NULL)
  {
    fprintf(stderr, "Error: Memory allocation for URL list failed.\n");
    fclose(fp);
    return 1;
  }

  char line[100];
  int count = 0;

  while (fgets(line, 100, fp) != NULL && count < MAX_N)
  {
    char *space = strchr(line, ' ');
    char url[MAX_ADDR_LENGTH];

    int numHops;

    // Storing url and numHops
    if (space != NULL)
    {
      int finalIndex = space - line;
      strncpy(url, line, finalIndex);
      url[finalIndex] = '\0';
      numHops = atoi(space + 1);

      // strdup allocates memory and copies the string
      urlList[count] = strdup(url);
      crawlURL(url, numHops, &count, MAX_N, trieList);
    }
  }

  for (int i = 0; i < count; i++)
  {
    if (urlList[i] != NULL) {
    free(urlList[i]);
    }
  }

  free(urlList);
  fclose(fp);

  return 0;
}

/* TODO: define the functions corresponding to the above prototypes */

/* TODO: change this return type */
TrieNode *indexPage(const char *url, int *totalNumTerms)
{
  *totalNumTerms = 0;

  TrieNode *root = malloc(sizeof(TrieNode));
  if (root == NULL)
  {
    printf("Failed memory allocation for TrieNode root\n");
    return NULL;
  }

  root->count = 0;
  for (int i = 0; i < 26; i++)
  {
    root->children[i] = NULL;
  }

  char *buffer = malloc(sizeof(char) * 30000);
  if (buffer == NULL)
  {
    printf("Failed memory allocation for char* buffer\n");
    free(root);
    return NULL;
  }
  int bytesRead = getText(url, buffer, 30000);
  if (bytesRead <= 0)
  {
    printf("This URL was not grabbed: %s\n", url);
    free(buffer);
    free(root);
    return NULL;
  }

  char **words = malloc(sizeof(char *) * 30000);
  if (words == NULL)
  {
    fprintf(stderr, "Memory allocation for words array failed\n");
    free(buffer);
    free(root);
    return NULL;
  }
  int wordCount = parseBuffer(buffer, words);

  for (int i = 0; i < wordCount; i++)
  {
    addWordOccurrence(root, words[i]);
    (*totalNumTerms)++;
    free(words[i]);
  }

  free(words);
  free(buffer);

  return root;
}

int addWordOccurrence(TrieNode *root, const char *words)
{

  TrieNode *currentNode = root;

  for (int i = 0; words[i] != '\0'; i++)
  {
    char ch = tolower(words[i]);

    if (ch < 'a' || ch > 'z')
    {
      fprintf(stderr, "Error: INVALID CHARACTER '%c' in word \"%s\"\n", ch, words);
      continue;
    }

    int index = ch - 'a';

    if (currentNode->children[index] == NULL)
    {
      currentNode->children[index] = malloc(sizeof(TrieNode));
      if (currentNode->children[index] == NULL)
      {
        fprintf(stderr, "Error: Memory allocation failed for TrieNode\n");
        return -1;
      }
      currentNode->children[index]->count = 0;
      for (int j = 0; j < 26; j++)
      {
        currentNode->children[index]->children[j] = NULL;
      }
    }
    currentNode = currentNode->children[index];
  }
  currentNode->count++;
  return 0;
}

void printTrieContents(TrieNode *root, char *buffer, int charCount)
{
  if (root == NULL)
  {
    return;
  }

  if (root->count > 0)
  {
    buffer[charCount] = '\0';
    printf("%s: %d\n", buffer, root->count);
  }

  for (int i = 0; i < 26; i++)
  {
    if (root->children[i] != NULL)
    {
      // Add character to buffer
      buffer[charCount] = 'a' + i;
      printTrieContents(root->children[i], buffer, charCount + 1);
    }
  }
}

int freeTrieMemory(TrieNode *node)
{
  if (node == NULL)
  {
    return 0;
  }
  for (int i = 0; i < 26; i++)
  {
    if (node->children[i])
    {
      freeTrieMemory(node->children[i]);
    }
  }
  free(node);
  return 0;
}

int getText(const char *srcAddr, char *buffer, const int bufSize)
{
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL)
  {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
            buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}

int parseBuffer(char *buffer, char **words)
{
  int count = 0;
  int wordCount = 0;

  while (buffer[count] != '\0')
  {

    // The parser has strings for bytes that are " b' " and were counted as words. This is the check for that.
    if (buffer[count] == 'b' && buffer[count + 1] == '\'')
    {
      count += 2;
      continue;
    }

    if (isalpha(buffer[count]))
    {
      int charCount = 0;

      while (isalpha(buffer[count]))
      {
        charCount++;
        count++;
      }

      words[wordCount] = malloc(sizeof(char) * (charCount + 1));

      if (words[wordCount] == NULL)
      {
        fprintf(stderr, "Memory allocation failed for word %d.\n", wordCount);
        return wordCount;
      }

      for (int i = 0; i < charCount; i++)
      {
        words[wordCount][i] = tolower(buffer[count - charCount + i]);
      }

      words[wordCount][charCount] = '\0';
      wordCount++;
    }

    count++;
  }

  return wordCount;
}

/*
 * returns 1 if the list starting at pNode contains the address addr,
 *    and returns 0 otherwise
 */
int contains(const struct listNode *pNode, const char *addr)
{
  // TODO: complete this

  if (pNode == NULL)
  {
    return 0;
  }

  if (strcmp(pNode->addr, addr) == 0)
  {
    return 1;
  }
  else
  {
    return contains(pNode->next, addr);
  }

  return 0;
}

/*
 * inserts the address addr as a new listNode at the end of
 *    the list
 */
void insertBack(struct listNode *pNode, const char *addr)
{
  // TODO: complete this

  struct listNode *newNode = (struct listNode *)malloc(sizeof(struct listNode));
  if (newNode == NULL)
  {
    printf("Could not allocate memory on heap in insertBack function");
    return;
  }

  strncpy(newNode->addr, addr, MAX_ADDR_LENGTH);
  newNode->next = NULL;

  if (pNode->next == NULL)
  {
    pNode->next = newNode;
    return;
  }
  else
  {
    insertBack(pNode->next, addr);
  }
}

/*
 * prints the addresses from pNode to the end of the list,
 *   one on each line
 */
void printAddresses(const struct listNode *pNode)
{
  if (pNode != NULL)
  {
    printf("%s\n", pNode->addr);
    printAddresses(pNode->next);
  }
}

/*
 * frees the memory associated with this node and all subsequent nodes
 */
void destroyList(struct listNode *pNode)
{
  // TODO: complete this

  if (pNode == NULL)
  {
    return;
  }

  destroyList(pNode->next); // Recursively free the next node first
  free(pNode);
}

int getLink(const char *srcAddr, char *link, const int maxLinkLength)
{
  const int bufSize = 1000;
  char buffer[bufSize];

  int numLinks = 0;

  FILE *pipe;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getLinks.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL)
  {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  fscanf(pipe, "%d\n", &numLinks);

  if (numLinks > 0)
  {
    int linkNum;
    double r = (double)rand() / ((double)RAND_MAX + 1.0);

    for (linkNum = 0; linkNum < numLinks; linkNum++)
    {
      fgets(buffer, bufSize, pipe);

      if (r < (linkNum + 1.0) / numLinks)
      {
        break;
      }
    }

    /* copy the address from buffer to link */
    strncpy(link, buffer, maxLinkLength);
    link[maxLinkLength - 1] = '\0';

    /* get rid of the newline */
    {
      char *pNewline = strchr(link, '\n');
      if (pNewline != NULL)
      {
        *pNewline = '\0';
      }
    }
  }

  pclose(pipe);

  if (numLinks > 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void crawlURL(char *url, int numHops, int *totalIndexed, int maxPages, TrieNode **trieList)
{
  if (*totalIndexed >= maxPages)
  {
    return;
  }

  char startAddr[MAX_ADDR_LENGTH];
  strncpy(startAddr, url, MAX_ADDR_LENGTH);

  int totalNumTerms = 0;
  TrieNode *root = indexPage(url, &totalNumTerms);

  if (root == NULL)
  {
    fprintf(stderr, "Error: Failed to index page: %s\n", url);
    return;
  }

  trieList[*totalIndexed] = root;

  printf("Indexing... %s\n", url);
  char buffer[30000];
  printTrieContents(root, buffer, 0);
  printf("Total terms: %d\n", totalNumTerms);
  printf("\n");

  struct listNode *pListStart = malloc(sizeof(struct listNode));

  if (pListStart == NULL)
  {
    printf("Error: Memory allocation for list node failed in crawlURL function.");
    freeTrieMemory(root);
    return;
  }

  strncpy(pListStart->addr, startAddr, MAX_ADDR_LENGTH);
  pListStart->next = NULL;

  for (int hopNum = 1; hopNum <= numHops && *totalIndexed < maxPages; hopNum++)
  {
    char destAddr[MAX_ADDR_LENGTH];
    int res = getLink(startAddr, destAddr, MAX_ADDR_LENGTH);

    if (!res)
    {
      printf("Dead end on hop %d: no outgoing links\n", hopNum);
      break;
    }

    if (contains(pListStart, destAddr))
    {
      printf("Cycle detected on hop %d: address %s\n", hopNum, destAddr);
      continue;
    }

    insertBack(pListStart, destAddr);
    strncpy(startAddr, destAddr, MAX_ADDR_LENGTH);
  }


  (*totalIndexed)++;
  destroyList(pListStart);
  freeTrieMemory(root);
  root = NULL;
}
