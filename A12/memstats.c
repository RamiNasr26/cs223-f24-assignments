#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "rand.h"

#define ROUNDS 3
#define BUFFER 5
#define LOOP 10

struct chunk {
  int size;
  int used;
  struct chunk *next;
};

void memstats(struct chunk* freelist, void* buffer[], int len) {
    int total_blocks = 0;
    int free_blocks = 0;
    int used_blocks = 0;
    int total_memory_allocated = 0;
    int total_memory_free = 0;
    int total_memory_used = 0;

    struct chunk* current = freelist;
    while (current != NULL) {
        total_blocks++;
        total_memory_allocated += current->size;
        
        if (current->used == 0) {
            free_blocks++;
            total_memory_free += current->size;
        } else {
            used_blocks++;
            total_memory_used += current->used;
        }
        
        current = current->next;
    }

    // Count used blocks in the buffer
    for (int i = 0; i < len; i++) {
        if (buffer[i] != NULL) {
            // Get the chunk header
            struct chunk* chunk = ((struct chunk*)buffer[i]) - 1;
            
            // If this block is not in the free list, count it
            int found_in_freelist = 0;
            current = freelist;
            while (current != NULL) {
                if (current == chunk) {
                    found_in_freelist = 1;
                    break;
                }
                current = current->next;
            }
            
            if (!found_in_freelist) {
                total_blocks++;
                total_memory_allocated += chunk->size;
                
                if (chunk->used > 0) {
                    used_blocks++;
                    total_memory_used += chunk->used;
                } else {
                    free_blocks++;
                    total_memory_free += chunk->size;
                }
            }
        }
    }

    // Calculate underutilized memory percentage
    double underutilized_percent = total_memory_allocated > 0 
        ? (double)(total_memory_allocated - total_memory_used) / total_memory_allocated 
        : 0.0;

    printf("Total blocks: %d Free blocks: %d Used blocks: %d\n", 
           total_blocks, free_blocks, used_blocks);
    printf("Total memory allocated: %d Free memory: %d Used memory: %d\n", 
           total_memory_allocated, total_memory_free, total_memory_used);
    printf("Underutilized memory: %.2f\n", underutilized_percent);
}

int main ( int argc, char* argv[]) {

  printf("Starting test..\n");

  srand(100);

  double timer;
  struct timeval tstart, tend;

  gettimeofday(&tstart, NULL);

  void *buffer[BUFFER];
  for (int i = 0; i < BUFFER; i++) {
    buffer[i] = NULL;
  }

  void *init = sbrk(0);
  void *current;
  printf("The initial top of the heap is %p.\n", init);
  for (int j = 0 ; j < ROUNDS; j++) {
    printf("---------------\n%d\n" , j);

    for (int i= 0 ; i < LOOP ; i++) {
      int index = rand() % BUFFER;
      if (buffer[index] != NULL) {
        free(buffer[index]);
        buffer[index] = NULL;
        printf("Freeing index %d\n", index);
      } 
      else {
        size_t size = (size_t) randExp(8, 4000); 
        int *memory = NULL;
        memory = malloc(size);

        if (memory == NULL) {
          fprintf(stderr, "malloc failed\n");
          return(1);
        } 
        *memory = 123;
        buffer[index] = memory;
        printf("Allocating %d bytes at index %d\n", (int) size, index);
      }
    }
    extern struct chunk* flist;
    current = sbrk(0);
    int allocated = current - init;
    init = current;

    printf("The new top of the heap is %p.\n", current);
    printf("Increased by %d (0x%x) bytes\n", allocated, allocated);
    memstats(flist, buffer, BUFFER);
  }

  for (int i = 0; i < BUFFER; i++) {
    free(buffer[i]); 
  }
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
  printf("Time is %g\n", timer);

  return 0 ;
}
