#include <unistd.h> // sbrk()
#include <string.h>  // memset
#include "allocator.h"
#include <stdio.h>

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

void* my_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size = ALIGN(size);  // ensure alignment

    // 1. Look for a free block in the free list
    block_header* block = find_free_block(size);

    // 2. If no free block is found, request more space
    if (block == NULL) {
        block = request_space(size);
        if (block == NULL) {
            return NULL; // sbrk failed
        }
    }

    // 3. If block is larger than needed → split it
    if (block->size >= size + sizeof(block_header) + ALIGNMENT) {
        split_block(block, size);
    }

    // 4. Remove the block from the free list because it's now in use
    remove_from_free_list(block);

    // 5. Mark it used in the heap list
    block->free = 0;

    // 6. Return pointer to memory AFTER the block header
    return (void*)((char*)block + sizeof(block_header));
}


void split_block(block_header* block, size_t size) {
    // new block begins right after allocated memory
    char* block_start = (char*)block;
    char* new_block_addr = block_start + sizeof(block_header) + size;

    block_header* new_block = (block_header*) new_block_addr;

    new_block->size = block->size - size - sizeof(block_header);
    new_block->free = 1;

    // Insert new_block into heap list
    new_block->next = block->next;
    new_block->prev = block;
    if (block->next) {
        block->next->prev = new_block;
    }
    block->next = new_block;

    // Adjust original block size
    block->size = size;

    // Insert new block into free list
    insert_into_free_list(new_block);
}