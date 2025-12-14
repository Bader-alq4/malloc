#include <stdio.h>
#include "../src/allocator.h"

extern block_header* heap_head;
extern block_header* free_head;

int main() {
    printf("=== Heap Structure Tests ===\n");

    // Request first block
    block_header* b1 = request_space(64);
    if (!b1) {
        printf("ERROR: request_space failed for b1\n");
        return 1;
    }
    printf("Allocated block 1 at %p (size=%zu)\n", (void*)b1, b1->size);

    // Check heap_head
    if (heap_head != b1)
        printf("ERROR: heap_head incorrect!\n");
    else
        printf("heap_head OK\n");

    // Check free list head
    if (free_head != b1)
        printf("ERROR: free_head incorrect!\n");
    else
        printf("free_head OK\n");

    // Request second block
    block_header* b2 = request_space(128);
    if (!b2) {
        printf("ERROR: request_space failed for b2\n");
        return 1;
    }
    printf("Allocated block 2 at %p (size=%zu)\n", (void*)b2, b2->size);

    // Verify heap linkage
    if (b1->next != b2 || b2->prev != b1)
        printf("ERROR: heap list linking incorrect!\n");
    else
        printf("Heap list linking OK\n");

    // Verify free list
    if (free_head != b2)
        printf("ERROR: free_head should be latest block (LIFO insertion)\n");
    else
        printf("Free list head OK\n");

    // Now test find_free_block
    block_header* found = find_free_block(100);
    if (found != b2)
        printf("ERROR: find_free_block should have returned b2\n");
    else
        printf("find_free_block OK (found correct block)\n");

    printf("\n=== All tests completed ===\n");

    return 0;
}
