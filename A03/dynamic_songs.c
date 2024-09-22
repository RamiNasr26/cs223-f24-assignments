/*----------------------------------------------
 * Author: Rami Nasr 
 * Date: 9/21/2024
 * Description: dynamic_songs.c reads song data from a CSV file (songlist.csv) into a dynamically allocated 
 * array of Song structures, extracting attributes like title, artist, and various metrics. 
 * It prints the song information in a formatted table and uses malloc for memory allocation and free to prevent leaks.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char title[128];
    char artist[128];
    int duration_ms;
    float danceability;
    float energy;
    float tempo;
    float valence;
} Song;

// Function to read songs from CSV into a dynamic array
Song* read_songs(const char *filename, int *num_songs) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }

    char line[256];
    fgets(line, sizeof(line), file); // First line with number of songs
    *num_songs = atoi(strtok(line, ","));  // First token is the number of songs

    // Skip the header line
    fgets(line, sizeof(line), file);

    // Allocate memory for the songs
    Song *songs = (Song *)malloc((*num_songs) * sizeof(Song));
    if (songs == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read each song line by line
    for (int i = 0; i < *num_songs; i++) {
        if (fgets(line, sizeof(line), file)) {
            // Tokenize each line
            char *token = strtok(line, ",");
            strncpy(songs[i].title, token, 127);

            token = strtok(NULL, ",");
            strncpy(songs[i].artist, token, 127);

            token = strtok(NULL, ",");
            songs[i].duration_ms = atoi(token);

            token = strtok(NULL, ",");
            songs[i].danceability = atof(token);

            token = strtok(NULL, ",");
            songs[i].energy = atof(token);

            token = strtok(NULL, ",");
            songs[i].tempo = atof(token);

            token = strtok(NULL, ",");
            songs[i].valence = atof(token);
        }
    }

    fclose(file);
    return songs;
}

// Function to print the song list
void print_songs(Song *songs, int num_songs) {
    printf("Welcome to Dynamic Donna's Danceability Directory.\n\n");
    for (int i = 0; i < num_songs; i++) {
        printf("%d) %-30s artist: %-20s duration: %d ms D: %.3f E: %.3f T: %.3f V: %.3f\n",
                i, songs[i].title, songs[i].artist, songs[i].duration_ms / 1000,
                songs[i].danceability, songs[i].energy, songs[i].tempo, songs[i].valence);
    }
}

int main() {
    int num_songs = 0;
    Song *songs = read_songs("songlist.csv", &num_songs);
    if (songs == NULL) {
        return 1;  // Error occurred
    }

    print_songs(songs, num_songs);

    free(songs);

    return 0;
}

