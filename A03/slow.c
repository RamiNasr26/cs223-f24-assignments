/*----------------------------------------------
 * Author:Rami Nasr 
 * Date: 9/20/2024
 * Description:
 * This program allows users to input a pause length (the number of dots between letters) and a phrase.
 * The output will be the phrase with the specified number of dots between each letter, including after the last letter.
 * For example, if the user inputs a pause length of '3' and the phrase 'You', the output will be:
 * Y...o...u...
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
  char buff[32];
  int n;
  printf("Pause length: ");
  scanf("%d", &n);
  printf("Text: ");
  scanf("%s", buff);

  int og_length = strlen(buff);
  int new_length = og_length * (n + 1);
  char *ent_phrase = malloc(new_length*sizeof(char)+ 1); //+1 for null terminator
  //Base case
  if (ent_phrase == NULL){
	  printf("Memory allocation failed!/n");
	  return 1;
  }
    
  int k = 0;
  for(int i = 0; i < og_length; i++){
  	ent_phrase[k++] = buff[i]; //Copying the character
	   for(int j = 0; j < n; j++){
	    ent_phrase[k++] = '.';
	   }
	}

  ent_phrase[k]= '\0';
  printf("%s\n", ent_phrase);
  free(ent_phrase);
  return 0;
}
