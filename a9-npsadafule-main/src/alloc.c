#include "alloc.h"
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#define HEADER_SIZE (sizeof(struct header))

static struct header *free_list = NULL;
static enum algs current_alg = FIRST_FIT;
static int heap_limit = 0;
static void *heap_start = NULL;
static int current_heap_size = 0;

// Helper function to reset allocator state
void reset_allocator() {
  free_list = NULL;
  current_heap_size = 0;
  heap_start = sbrk(0);
}

// Function to coalesce contiguous free blocks
void coalesce_free_blocks() {
  struct header *curr = free_list;
  while (curr && curr->next) {
    if ((void *)((char *)curr + HEADER_SIZE + curr->size) == curr->next) {
      curr->size += HEADER_SIZE + curr->next->size;
      curr->next = curr->next->next;
    } else {
      curr = curr->next;
    }
  }
}

// Allocation function
void *alloc(int size) {
  if (size <= 0) {
    return NULL;
  }

  struct header *prev = NULL;
  struct header *curr = free_list;
  struct header *best_fit = NULL;
  struct header *best_fit_prev = NULL;

  // Find a suitable block based on the allocation algorithm
  while (curr) {
    if (curr->size >= size) {
      if (current_alg == FIRST_FIT) {
        break;
      } else if (current_alg == BEST_FIT) {
        if (!best_fit || curr->size < best_fit->size) {
          best_fit = curr;
          best_fit_prev = prev;
        }
      } else if (current_alg == WORST_FIT) {
        if (!best_fit || curr->size > best_fit->size) {
          best_fit = curr;
          best_fit_prev = prev;
        }
      }
    }
    prev = curr;
    curr = curr->next;
  }

  if (current_alg != FIRST_FIT) {
    curr = best_fit;
    prev = best_fit_prev;
  }

  // If no suitable block is found, request more memory from the OS
  if (!curr) {
    if (heap_limit != 0 && current_heap_size + INCREMENT > heap_limit) {
      return NULL;
    }
    struct header *new_block = (struct header *)sbrk(INCREMENT);
    if (new_block == (void *)-1) {
      return NULL;
    }
    new_block->size = INCREMENT - HEADER_SIZE;
    new_block->next = NULL;
    current_heap_size += INCREMENT;

    // Add the new block to the free list
    dealloc((void *)((char *)new_block + HEADER_SIZE));
    curr = new_block;
    prev = NULL;
  }

  // Split the block if it is significantly larger than needed
  if (curr->size > size + HEADER_SIZE) {
    struct header *new_block =
        (struct header *)((char *)curr + HEADER_SIZE + size);
    new_block->size = curr->size - size - HEADER_SIZE;
    new_block->next = curr->next;
    curr->size = size;

    // Update the free list
    if (prev) {
      prev->next = new_block;
    } else {
      free_list = new_block;
    }
  } else {
    // Remove the block from the free list if it cannot be split
    if (prev) {
      prev->next = curr->next;
    } else {
      free_list = curr->next;
    }
  }

  curr->next =
      NULL; // Ensure that the allocated block doesn't point to any free block

  return (void *)((char *)curr + HEADER_SIZE);
}

// Deallocation function
void dealloc(void *ptr) {
  if (!ptr) {
    return;
  }

  struct header *block = (struct header *)((char *)ptr - HEADER_SIZE);

  // Insert the block into the free list in sorted order
  struct header *curr = free_list;
  struct header *prev = NULL;

  while (curr && curr < block) {
    prev = curr;
    curr = curr->next;
  }

  // Insert the block into the list
  block->next = curr;
  if (prev) {
    prev->next = block;
  } else {
    free_list = block;
  }

  // Coalesce contiguos free blocks to avoid fragmentation
  coalesce_free_blocks();
}

// Set allocator options
void allocopt(enum algs alg, int limit) {
  current_alg = alg;
  heap_limit = limit;
  reset_allocator();
}

// Get allocation info
struct allocinfo allocinfo(void) {
  struct allocinfo info = {0};
  struct header *curr = free_list;

  while (curr) {
    info.free_size += curr->size;
    info.free_chunks++;
    if (curr->size > info.largest_free_chunk_size) {
      info.largest_free_chunk_size = curr->size;
    }
    if (info.smallest_free_chunk_size == 0 ||
        curr->size < info.smallest_free_chunk_size) {
      info.smallest_free_chunk_size = curr->size;
    }
    curr = curr->next;
  }

  return info;
}
