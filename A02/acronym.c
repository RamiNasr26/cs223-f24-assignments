
/**
* Acronym File
*
* This program allows users to enter a phrase to find its acronym.
* It replaces the first letter (which is capital) of each word in the phrase in an acronym.
*
* @author: Rami Nasr
* @Date: 9/14/2024
*/
#include <stdio.h>
int main() {
    printf("Enter a phrase: \n");
    char phrase[1024];   
    char acronym[512]; 
    int j = 0;       
    scanf(" %[^\n]", phrase);
    // Loop through each character in the phrase
    for (int i = 0; phrase[i] != '\0'; i++) {
        // Check if the character is an uppercase letter (A-Z)
        if (phrase[i] >= 65 && phrase[i] <= 90) {
            acronym[j] = phrase[i];  // Store the capital letter in the acronym
            j++;                     
        }
    }
    acronym[j] = '\0';  
    printf("Your acronym is: %s\n", acronym);
    return 0;
}
