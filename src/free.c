#include <stddef.h>
#include "allocator.h"

extern block_header* free_head;

// Remove a block from the free list
void remove_from_free_list(block_header* block) {
    if (block->prev_free) {
        block->prev_free->next_free = block->next_free;
    } else {
        free_head = block->next_free;
    }

    if (block->next_free) {
        block->next_free->prev_free = block->prev_free;
    }

    block->next_free = NULL;
    block->prev_free = NULL;
}


// Merge helpers
block_header* merge_with_prev(block_header* block) {
    block_header* prev = block->prev;

    // Remove prev from free list (it will become the merged block)
    remove_from_free_list(prev);

    // Expand prev to absorb block
    prev->size += sizeof(block_header) + block->size;

    // Fix heap list
    prev->next = block->next;
    if (block->next) {
        block->next->prev = prev;
    }

    return prev;  // merged block is now prev
}


void merge_with_next(block_header* block) {
    block_header* next = block->next;

    // Remove next free block from free list
    remove_from_free_list(next);

    // Increase size
    block->size += sizeof(block_header) + next->size;

    // Fix heap pointers
    block->next = next->next;
    if (next->next) {
        next->next->prev = block;
    }
}


// Main coalesce function
block_header* coalesce(block_header* block) {

    // If merging backward, block becomes prev
    if (block->prev && block->prev->free) {
        // Remove the current block from free list
        remove_from_free_list(block);

        // Merge with prev
        block = merge_with_prev(block);

        // Insert merged block back into free list
        insert_into_free_list(block);
    }

    // If merging forward, block absorbs next
    if (block->next && block->next->free) {
        // next is consumed; remove it
        merge_with_next(block);
        // block stays the merged block; just keep it in free list
    }

    return block;
}


// my_free implementation
void my_free(void* ptr) {
    if (ptr == NULL) return;

    // Get the block header
    block_header* block = (block_header*)((char*)ptr - sizeof(block_header));

    block->free = 1;

    // Insert block into free list
    insert_into_free_list(block);

    // update block pointer after coalescing
    block = coalesce(block);
}
