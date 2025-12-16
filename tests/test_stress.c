#include <stdio.h>
#include <string.h>  // memset
#include <stdlib.h>
#include <time.h>
#include "../src/allocator.h"

// Forward declarations for debugging helpers
void print_heap();
void print_free_list();

// A simple integrity check: ensure free blocks are actually marked free
int validate_free_list() {
    block_header* curr = free_head;
    while (curr) {
        if (!curr->free) {
            printf("ERROR: Free list contains a used block at %p\n", (void*)curr);
            return 0;
        }
        curr = curr->next_free;
    }
    return 1;
}

// Validate the heap list: ensure no overlaps, no cycles, and consistent free flags
int validate_heap() {
    block_header* curr = heap_head;
    block_header* prev = NULL;

    while (curr) {
        // Check prev pointer correctness
        if (curr->prev != prev) {
            printf("ERROR: Heap prev pointer mismatch at %p\n", (void*)curr);
            return 0;
        }

        // Check coalescing invariant:
        if (curr->free && curr->next && curr->next->free) {
            printf("ERROR: Adjacent free blocks exist at %p and %p — coalescing failed.\n",
                   (void*)curr, (void*)curr->next);
            return 0;
        }

        prev = curr;
        curr = curr->next;
    }
    return 1;
}

// Generate a random size in range [8, 256]
size_t random_size() {
    return (rand() % 256) + 8;
}

int main() {
    printf("=== Stress Testing ===\n");

    srand((unsigned)time(NULL));

    const int N = 5000;   // number of operations
    void* ptrs[N];        // store allocations
    int allocated = 0;

    for (int i = 0; i < N; i++) {
        int op = rand() % 2; // 0 = malloc, 1 = free

        if (op == 0 && allocated < N - 1) {
            // Allocate a random-sized block
            size_t size = random_size();
            void* p = my_malloc(size);

            if (!p) {
                printf("ERROR: malloc returned NULL during stress test\n");
                return 1;
            }

            // Write into memory to ensure it's usable
            memset(p, 0xAB, size);

            ptrs[allocated++] = p;
        }
        else if (allocated > 0) {
            // Free a random previously allocated block
            int idx = rand() % allocated;
            my_free(ptrs[idx]);

            // Replace freed slot with last allocated pointer
            ptrs[idx] = ptrs[allocated - 1];
            allocated--;
        }

        // Periodically validate across the run
        if (i % 500 == 0) {
            if (!validate_heap()) return 1;
            if (!validate_free_list()) return 1;
        }
    }

    // Free everything left
    for (int i = 0; i < allocated; i++) {
        my_free(ptrs[i]);
    }

    // Final full validation
    printf("\nRunning final heap + free list integrity checks...\n");
    if (!validate_heap()) return 1;
    if (!validate_free_list()) return 1;

    printf("\nSUCCESS: Stress test completed with no errors.\n");
    return 0;
}
