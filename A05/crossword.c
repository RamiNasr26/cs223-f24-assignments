/*----------------------------------------------
 * Author: Rami Nasr
 * Date: 10/5/2024
 * Description: The crossword.c program creates a crossword using two input words.
 * It finds a common letter between them, places the first word vertically and the second horizontally 
 * at the common letter. If no common letter is found, it prints "No common letter!" and exits.
 * It uses dynamic memory allocation for the crossword grid.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find_common_letter(char* word1, char* word2, int* index1, int* index2) {
    for (int i = 0; i < strlen(word1); i++) {
        for (int j = 0; j < strlen(word2); j++) {
            if (word1[i] == word2[j]) {
                *index1 = i;
                *index2 = j;
                return 1;
            }
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <word1> <word2>\n", argv[0]);
        return 0;
    }

    char* word1 = argv[1];
    char* word2 = argv[2];

    int index1 = -1, index2 = -1;
    if (!find_common_letter(word1, word2, &index1, &index2)) {
        printf("No common letter!\n");
        return 0;
    }

    // Determine grid size
    int rows = strlen(word1);
    int cols = strlen(word2);

    // Allocate 2D array
    char** crossword = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        crossword[i] = (char*)malloc(cols * sizeof(char));
        for (int j = 0; j < cols; j++) {
            crossword[i][j] = '.';  // Fill grid with dots
        }
    }

    // Insert word1 vertically at column index2
    for (int i = 0; i < strlen(word1); i++) {
        crossword[i][index2] = word1[i];
    }

    // Insert word2 horizontally at row index1
    for (int j = 0; j < strlen(word2); j++) {
        crossword[index1][j] = word2[j];
    }

    // Print the crossword
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", crossword[i][j]);
        }
        printf("\n");
    }

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(crossword[i]);
    }
    free(crossword);

    return 0;
}

