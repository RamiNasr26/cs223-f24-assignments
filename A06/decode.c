/*----------------------------------------------
 * Author: Rami Nasr
 * Date: 10/11/2024
 * Description: This program extracts a hidden message from the least significant bits (LSBs) of the red, green,
 * and blue color channels of each pixel in a PPM image. 
 * It reads the image using read_ppm, then decodes the message by combining bits 
 * into characters until the null character (\0) is found
 * ----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: decode <file.ppm>\n");
        return 0;
    }

    int width, height;
    struct ppm_pixel* pixels = read_ppm(argv[1], &width, &height);
    if (pixels == NULL) {
        printf("Error reading PPM file.\n");
        return 1;
    }

    printf("Reading %s with width %d and height %d\n", argv[1], width, height);

    // Calculate the maximum number of characters that can be stored
    int max_chars = (width * height * 3) / 8; // 3 colors per pixel, 8 bits per character
    printf("Max number of characters in the image: %d\n", max_chars);

    // Extract the message from the LSBs
    unsigned char current_char = 0;
    int bit_count = 0;
    
    for (int i = 0; i < width * height; i++) {
        // Extract LSBs from red, green, and blue
        unsigned char bits[3];
        bits[0] = pixels[i].red & 1;    // LSB of red
        bits[1] = pixels[i].green & 1;  // LSB of green
        bits[2] = pixels[i].blue & 1;   // LSB of blue

        // Append bits to current_char
        for (int k = 0; k < 3; k++) {
            current_char = (current_char << 1) | bits[k];
            bit_count++;

            // If we've read 8 bits, we have a full character
            if (bit_count == 8) {
                if (current_char == '\0') {
                    printf("\n"); // End of message
                    free_ppm(pixels); // Free memory
                    return 0;
                }
                printf("%c", current_char);
                current_char = 0;
                bit_count = 0;
            }
        }
    }

    printf("\n");
    free_ppm(pixels); // Free memory
    return 0;
}

