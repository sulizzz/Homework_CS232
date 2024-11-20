/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


typedef struct TrieNode{
  int count; 
  struct TrieNode* children[26];
// for the alphabet of 26 letters
}TrieNode;

/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

/* TODO: change this return type */
char* indexPage(const char* url);

int addWordOccurrence(TrieNode* root, const char* words);

void printTrieContents(/* TODO: any parameters you need */);

int freeTrieMemory(/* TODO: any parameters you need */);

int getText(const char* srcAddr, char* buffer, const int bufSize);

int parseBuffer(char* buffer, char** words);

int main(int argc, char** argv){
  char* buffer = indexPage(argv[1]);

  TrieNode* root = malloc(sizeof(TrieNode)); 
  root->count = 0;
  for (int i = 0; i < 26; i++) {
    root->children[i] = NULL;
  }

  char** words = malloc(sizeof(char*) * 30000);

  int wordCount = parseBuffer(buffer, words);

  for (int i = 0; i < wordCount; i++) {
    addWordOccurrence(root, words[i]);
  }

  char newBuffer[30000]; 
  printf("\n\n");
  printTrieContents(root, newBuffer, 0);

  freeTrieMemory(root);
  free(buffer);
  free(words);  

  return 0;
}

/* TODO: define the functions corresponding to the above prototypes */

/* TODO: change this return type */
char* indexPage(const char* url)
{
  char* buffer = malloc(sizeof(char) * 30000);

  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation for buffer did not work\n");
    return NULL;
  }

  // Get text into buffer
  int n = getText(url, buffer, 30000);
    
  // Check if getText was got the text
  if (n > 0) {
    printf("Fetched content:\n%s\n", buffer);
  } else {
    fprintf(stderr, "Failed to get the text from URL in getText Function\n");
  }

  return buffer;
}

int addWordOccurrence(TrieNode* root, const char* words){

    TrieNode* currentNode = root;

    for(int i = 0; words[i] != '\0'; i++){
      char ch = tolower(words[i]);

      if(ch < 'a' || ch > 'z'){
        fprintf(stderr, "Error: INVALID CHARACTER '%c' in word \"%s\"\n", ch, words);
        continue;
      }

      int index = ch - 'a';

      if(currentNode->children[index] == NULL){
         currentNode->children[index] = malloc(sizeof(TrieNode));
         if (currentNode->children[index] == NULL) {
          fprintf(stderr, "Error: Memory allocation failed for TrieNode\n");
          return -1;
         }
         currentNode->children[index]->count = 0;
         for(int j = 0; j < 26; j++){
            currentNode->children[index]->children[j] = NULL;
         }
    }
    currentNode = currentNode->children[index];

    }
    currentNode->count++;
    return 0;
}

void printTrieContents(TrieNode* root, char* buffer, int charCount) {
  if (root == NULL) {
    return;
  }

  if (root->count > 0) {
    buffer[charCount] = '\0';
    printf("%s: %d\n", buffer, root->count);
  }

  for (int i = 0; i < 26; i++) {
        if (root->children[i] != NULL) {
          // Add character to buffer
          buffer[charCount] = 'a' + i; 
          printTrieContents(root->children[i], buffer, charCount + 1);
        }
    }

}

int freeTrieMemory(TrieNode* node){
  if (node == NULL){
    return 0;
  }
  for (int i = 0; i < 26; i++){
    if(node->children[i]){
      freeTrieMemory(node->children[i]);
    }
  }
  free(node);
  return 0;
}

int getText(const char* srcAddr, char* buffer, const int bufSize){
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
	    buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}

int parseBuffer(char* buffer, char** words) {
  int count = 0;
  int wordCount = 0;

  while (buffer[count] != '\0') {

    // The parser has strings for bytes that are " b' " and were counted as words. This is the check for that.
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



