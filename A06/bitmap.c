/*----------------------------------------------
 * Author: 
 * Date: 
 * Description
 ---------------------------------------------*/
#include <stdio.h>

int main() {
    unsigned long img;

    // Read the 64-bit hexadecimal input
    scanf(" %lx", &img);
    printf("Image (unsigned long): %lx\n", img);

    // Mask to extract each bit starting from the most significant bit (left-most)
    unsigned long mask = 0x1ul << 63;

    // Loop through all 64 bits and display as an 8x8 bitmap
    for (int i = 0; i < 64; i++) {
        // Check the current bit and print '@' for 1 (black), ' ' for 0 (white)
        if (img & mask) {
            printf("@ ");
        } else {
            printf("  ");
        }

        // Shift the mask to the right to check the next bit
        mask >>= 1;

        // After every 8 bits, move to the next line for an 8x8 grid
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }

    return 0;
}

