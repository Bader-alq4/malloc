// defines the structure, global variables, and function prototypes 
// that the entire allocator relies on. It is the public contract 
// for how the malloc/free system works.

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>   // for size_t


// Block Header Structure
// One block participates in *two* linked lists:
// 1. The HEAP LIST (prev, next) -> Physical memory order
// 2. The FREE LIST (prev_free, next_free) -> Only free blocks

typedef struct block_header {
    size_t size; // Size of the user memory
    int free; // 1 = free, 0 = used

    // Heap list pointers (physical order)
    struct block_header* next;
    struct block_header* prev;

    // Free list pointers (logical list of free blocks only)
    struct block_header* next_free;
    struct block_header* prev_free;
} block_header;


// ---------------------------------------------------------------------------
// Global Heads
// ---------------------------------------------------------------------------

// First block in the *heap list*
extern block_header* heap_head;

// Head of the *free list* (first free block). Only blocks with free = 1 are in the free list.
extern block_header* free_head;


// Alignment Helper (align sizes to 16 bytes)

// Round size up to nearest 16 bytes for proper alignment. Lower 4 bytes are set to 0.
// This is used to ensure that the size of the block is a multiple of 16 bytes. Power-of-Two Alignment Masking
// ~ means bitwise NOT
#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))


// Public API

// Custom malloc and free
void* my_malloc(size_t size);
void my_free(void* ptr);


// Internal Helpers

// malloc() and free() use the free list. Coalesce, split, metadata logic, and pyhysical adjacency logic are used in the heap list

// Search the free list for a block that fits the request. Malloc() uses the free list to search for blocks
block_header* find_free_block(size_t size);

// Request more memory from the OS via sbrk()
block_header* request_space(size_t size);

// Insert a block into the free list
void insert_into_free_list(block_header* block);

// Remove a block from the free list
void remove_from_free_list(block_header* block);

// Split a block into used/free parts
void split_block(block_header* block, size_t size);

// Merge adjacent free blocks in the heap list
void coalesce(block_header* block);


#endif // ALLOCATOR_H