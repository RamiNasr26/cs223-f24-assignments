/*----------------------------------------------
 * Author: Rami Nasr
 * Date: 10/11/2024
 * Description: This program embeds a user-provided message into the least significant bits (LSBs) of the red, green, and blue channels of each pixel in a PPM image. 
 * It reads the image using read_ppm, modifies the pixel data to hide the message, and then saves the modified image to a new file. 
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: encode <file.ppm>\n");
        return 0;
    }

    int width, height;
    struct ppm_pixel* pixels = read_ppm(argv[1], &width, &height);
    if (pixels == NULL) {
        printf("Error reading PPM file.\n");
        return 1;
    }

    printf("Reading %s with width %d and height %d\n", argv[1], width, height);

    int max_chars = (width * height * 3) / 8; // 3 colors per pixel, 8 bits per character
    printf("Max number of characters that can be stored: %d\n", max_chars);

    char message[256];
    printf("Enter a phrase to encode (max %d characters): ", max_chars);
    fgets(message, sizeof(message), stdin);
    int message_length = strlen(message);

    if (message_length > max_chars) {
        printf("Error: The message is too long to encode in this image.\n");
        free_ppm(pixels);
        return 1;
    }

    
    int char_index = 0;
    int bit_index = 0;
    for (int i = 0; i < width * height; i++) {
        if (char_index >= message_length) break;

        unsigned char bits[3];
        bits[0] = (message[char_index] >> (7 - bit_index)) & 1; 
        pixels[i].red = (pixels[i].red & 0xFE) | bits[0];        

        if (++bit_index == 8) {
            bit_index = 0;
            char_index++;
            if (char_index >= message_length) break;
        }

        bits[1] = (message[char_index] >> (7 - bit_index)) & 1; 
        pixels[i].green = (pixels[i].green & 0xFE) | bits[1];   

        if (++bit_index == 8) {
            bit_index = 0;
            char_index++;
            if (char_index >= message_length) break;
        }

        bits[2] = (message[char_index] >> (7 - bit_index)) & 1; 
        pixels[i].blue = (pixels[i].blue & 0xFE) | bits[2];     

        if (++bit_index == 8) {
            bit_index = 0;
            char_index++;
        }
    }

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s-encoded.ppm", argv[1]);
    write_ppm(output_filename, pixels, width, height);
    printf("Message encoded and written to %s\n", output_filename);

    free_ppm(pixels); 
    return 0;
}

