#include <unistd.h> // sbrk()
#include <string.h>  // memset
#include "allocator.h"

block_header* heap_head = NULL;
block_header* free_head = NULL;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

block_header* request_space(size_t size) {
    // Ask OS for memory: block header + user space
    size_t total_size = sizeof(block_header) + size;
    void* new_mem = sbrk(total_size);

    if (new_mem == (void*) -1) {
        return NULL;  // sbrk failed
    }

    // Initialize the new block
    block_header* block = (block_header*) new_mem;
    block->size = size;
    block->free = 1;

    // Heap list pointers
    block->next = NULL;
    block->prev = NULL;

    // Free list pointers
    block->next_free = NULL;
    block->prev_free = NULL;

    // Insert into Heap List
    if (heap_head == NULL) {
        heap_head = block;
    } else {
        block_header* last = heap_head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = block;
        block->prev = last;
    }

    // Insert into Free List
    insert_into_free_list(block);

    return block;
}

void insert_into_free_list(block_header* block) {
    block->next_free = free_head;
    block->prev_free = NULL;

    if (free_head != NULL) {
        free_head->prev_free = block;
    }

    free_head = block;
}

// Search the tree for a free block that fits the request. 
// Malloc() uses the free list to search for blocks using the first-fit strategy
block_header* find_free_block(size_t size) {
    block_header* curr = free_head;

    while (curr != NULL) {
        if (curr->size >= size) {
            return curr;  // Found a suitable block
        }
        curr = curr->next_free;
    }

    return NULL;  // No suitable block found
}
