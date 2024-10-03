#include <stdio.h>

//Before compiling/running the program, try and identify
// at least 3 bugs. Write your guesses below:
//1. the ints are being assigned characters
//2. the printf statement does not have put %d
//3. there is no return statement

int main (){
  int width, area, height = 0;

  printf("Please enter width of rectangle (< 10):\n");
  width = getchar() - '0';
  getchar();

  printf("Please enter width of rectangle (< 10):\n");
  height = getchar() - '0';
  getchar();

  area = width * height;

  printf("The area of the rectangle is: %d\n", area);

  return 0;

}



