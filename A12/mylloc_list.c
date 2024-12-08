#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct chunk {
  int size;
  int used;
  struct chunk *next;
};

struct chunk *flist = NULL;

void *malloc(size_t size) {
  if (size == 0) return NULL;

  struct chunk *prev = NULL;
  struct chunk *current = flist;
  
  while (current != NULL) {
    if (current->size >= size && current->used == 0) {
      current->used = size;
      
      if (current == flist) {
        flist = current->next;
      } else if (prev != NULL) {
        prev->next = current->next;
      }
      
      return (void*)(current + 1);
    }
    
    prev = current;
    current = current->next;
  }
  
  struct chunk *new_chunk = sbrk(sizeof(struct chunk) + size);
  if (new_chunk == (void*) -1) {
    return NULL;
  }
  
  new_chunk->size = size;
  new_chunk->used = size;
  new_chunk->next = NULL;
  
  return (void*)(new_chunk + 1);
}

void free(void *memory) {
  if (memory == NULL) return;
  
  struct chunk *chunk = ((struct chunk*)memory) - 1;
  
  chunk->used = 0;
  
  chunk->next = flist;
  flist = chunk;
}
