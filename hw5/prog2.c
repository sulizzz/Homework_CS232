#include <stdio.h>
#include <string.h>

//Before compiling/running the program, try and identify
// at least 2 bugs. Write your guesses below:
//1. j should start at strlen to make sure it doesn't access a different memory address
//2. putchar is inputting a string not a char


int main(){
  char name[] = {'T', 'i', 'm', 'o', 't', 'h', 'y'};

  printf("Hello %s\n", name);

  printf("Your name backwards is:\n");
  for(int j = strlen(name) - 1; j >= 0; j--){
    putchar(name[j]);

  }
  
  putchar('\n');

  return 0;

}
