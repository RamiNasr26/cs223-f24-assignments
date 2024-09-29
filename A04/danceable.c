/*----------------------------------------------
 * Author: Rami Nasr 
 * Date: 9/27/2024
 * Description: This program makes a linked list database for the songlist.csv dataset. The user has a choice to show
 * the list of songs, press 'd' to show the most danceable song, or press 'q' to quit the program.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

typedef struct {
    char *title;
    char *artist;
    int duration;
    float danceability;
    float energy;
    float tempo;
    float valence;
} Song;

typedef struct Node {
    Song *song;
    struct Node *next;
} Node;

// Function to create a new linked list node
Node* create_node(Song *song) {
    Node *new_node = malloc(sizeof(Node));
    new_node->song = song;
    new_node->next = NULL;
    return new_node;
}

// Insert song into the linked list
void insert_song(Node **head, Song *song) {
    Node *new_node = create_node(song);
    new_node->next = *head;
    *head = new_node;
}

// Print all the songs in the linked list
void print_songs(Node *head) {
    Node *current = head;
    int index = 0;
    while (current != NULL) {
        Song *song = current->song;
        printf("%d) %s\t%s\t(%d:%02ds) D: %.3f E: %.3f T: %.3f V: %.3f\n",
               index++, song->title, song->artist, song->duration / 60000,
               (song->duration % 60000) / 1000, song->danceability, song->energy,
               song->tempo, song->valence);
        current = current->next;
    }
}

// Function to load songs from the CSV file and insert them into the linked list
void load_songs(Node **head, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    fgets(line, sizeof(line), file);  // Skip the header line

    while (fgets(line, sizeof(line), file)) {
        // Dynamically allocate memory for a new song
        Song *song = malloc(sizeof(Song));

        // Parse each field in the line
        char *token = strtok(line, ",");
        song->title = strdup(token);

        token = strtok(NULL, ",");
        song->artist = strdup(token);

        token = strtok(NULL, ",");
        song->duration = atoi(token);

        token = strtok(NULL, ",");
        song->danceability = atof(token);

        token = strtok(NULL, ",");
        song->energy = atof(token);

        token = strtok(NULL, ",");
        song->tempo = atof(token);

        token = strtok(NULL, ",");
        song->valence = atof(token);

        // Insert the parsed song into the linked list
        insert_song(head, song);
    }

    fclose(file);
}

// Find the most danceable song in the linked list
Node* find_most_danceable(Node *head) {
    Node *current = head;
    Node *most_danceable_node = head;

    while (current != NULL) {
        if (current->song->danceability > most_danceable_node->song->danceability) {
            most_danceable_node = current;
        }
        current = current->next;
    }
    return most_danceable_node;
}

// Remove a specific node from the linked list
void remove_node(Node **head, Node *target) {
    Node *current = *head;
    Node *prev = NULL;

    if (*head == target) {
        *head = target->next;
        free(target->song->title);
        free(target->song->artist);
        free(target->song);
        free(target);
        return;
    }

    while (current != NULL) {
        if (current == target) {
            prev->next = current->next;
            free(current->song->title);
            free(current->song->artist);
            free(current->song);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Clear the linked list and free all memory
void clear_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->song->title);
        free(current->song->artist);
        free(current->song);
        free(current);
        current = next;
    }
}

int main() {
    Node *song_list = NULL;
    load_songs(&song_list, "songlist.csv");

    char choice;
    do {
        printf("0) Show song list\n");
        printf("d) Show the most danceable song\n");
        printf("q) Quit\n");
        printf("Choice: ");
        scanf(" %c", &choice);

        if (choice == '0') {
            print_songs(song_list);
        } else if (choice == 'd') {
            if (song_list == NULL) {
                printf("No songs available.\n");
            } else {
                Node *most_danceable = find_most_danceable(song_list);
                printf("---------------------------- Most danceable ----------------------------\n");
                Song *song = most_danceable->song;
                printf("%s\t%s\t(%d:%02ds) D: %.3f E: %.3f T: %.3f V: %.3f\n",
                       song->title, song->artist, song->duration / 60000,
                       (song->duration % 60000) / 1000, song->danceability,
                       song->energy, song->tempo, song->valence);
                printf("-------------------------------------------------------------------------\n");

                remove_node(&song_list, most_danceable);
            }
        }
    } while (choice != 'q');

    clear_list(song_list);

    return 0;
}

