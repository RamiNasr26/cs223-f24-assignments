/***************************************************
 * mathgame.c
 * Author: Rami Nasr
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h> 

int main() {
  printf("Welcome to Math Game\nHow many rounds to do you want to play?\n");
  int rounds = 0;
  scanf("%d", &rounds);
  int score = 0;
  for(int i = 0; i < rounds; i++) {
	  int x = (rand() % 9) + 1;
	  int y = (rand() % 9) + 1;
	  int ans = x + y;
	  int response = 0;
	  printf("%d + %d = ", x, y);
	  scanf("%d", &response);
	  if (response == ans) { 
		  printf("Correct :)\n");
	  	  score ++;

	  } else {
		  printf("Incorrect :(\n");
	  }	
  }
	printf("You answered %d/%d correctly!\n", score, rounds);	 
	return 0;
}
