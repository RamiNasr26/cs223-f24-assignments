/**
* Songs File
*
* This program allows users to view a list of songs with details such as artist, 
* title, duration, and danceability. Users can select a song to edit its attributes, 
* including the artist, title, duration, or danceability.
*
* @author: Rami Nasr
* @Date: 9/14/2024
*/

#include <stdio.h>
#include <string.h>
struct song {
    int duration_minutes;
    int duration_seconds;
    float danceability;
    char artist[64];
    char title[64];
};

// Function prototypes
void print_songs(struct song songs[], int size);
void edit_song(struct song *s);

int main() {
    struct song songs[3] = {
        {4, 1, 0.85, "J. Cole", "Land of the Snakes"},
        {3, 35, 0.9, "Kendrick Lamar", "PRIDE."},
        {5, 34, 0.30, "XXXTENTACION", "Vice City"}
    };

    print_songs(songs, 3);

    int song_id;
    printf("Enter a song id to edit [0,1,2]: ");
    scanf("%d", &song_id);

    if (song_id >= 0 && song_id < 3) {
        edit_song(&songs[song_id]);
        print_songs(songs, 3);
    } else {
        printf("Invalid song id.\n");
    }

    return 0;
}

void print_songs(struct song songs[], int size) {
    printf("\n===============================\n");
    for (int i = 0; i < size; i++) {
        printf("%d) %-20s artist: %-20s duration: %d:%02d danceability: %.2f\n", 
               i, songs[i].title, songs[i].artist, songs[i].duration_minutes, 
               songs[i].duration_seconds, songs[i].danceability);
    }
    printf("===============================\n");
}

void edit_song(struct song *s) {
    char attribute[64];
    printf("Which attribute do you wish to edit? [artist, title, duration, danceability]: ");
    scanf("%s", attribute);

    if (strcmp(attribute, "artist") == 0) {
        printf("Enter new artist: ");
        scanf(" %[^\n]", s->artist);
    } else if (strcmp(attribute, "title") == 0) {
        printf("Enter new title: ");
        scanf(" %[^\n]", s->title);
    } else if (strcmp(attribute, "duration") == 0) {
        printf("Enter new duration (minutes): ");
        scanf("%d", &s->duration_minutes);
        printf("Enter new duration (seconds): ");
        scanf("%d", &s->duration_seconds);
    } else if (strcmp(attribute, "danceability") == 0) {
        printf("Enter new danceability: ");
        scanf("%f", &s->danceability);
    } else {
        printf("Invalid attribute.\n");
    }
}

