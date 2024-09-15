/**
* Exclaim File
* 
* This program replaces lowercase letters with a number of different exclamation marks
*
* @author: Rami Nasr
* @Date: 9/14/2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // Initialize random seed
    srand(time(NULL));
    printf("Enter a word: \n");
    char buff[32]; 
    scanf("%s", buff);
    // Array of exclamation-like characters
    char exclamations[] = {'!', '@', '#', '$', '%'};
    // Loop through each character in the word
    for (int i = 0; buff[i] != '\0'; i++) {
        // Check if the character is a lowercase letter (ASCII 97-122)
        if (buff[i] >= 97 && buff[i] <= 122) {
            // Replace lowercase letter with a random exclamation-like character
            int random_index = rand() % 5;
            buff[i] = exclamations[random_index];
        }
    }

    printf("OMG! %s\n", buff); 

    return 0;
}
