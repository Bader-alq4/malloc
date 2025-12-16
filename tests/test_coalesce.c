#include <stdio.h>
#include "../src/allocator.h"

// These are defined in malloc.c
extern block_header* heap_head;
extern block_header* free_head;

// Debug printer for heap list
void print_heap() {
    printf("\n--- HEAP LIST ---\n");
    block_header* curr = heap_head;
    int i = 0;

    while (curr != NULL) {
        printf("Block %d: addr=%p  size=%zu  free=%d  next=%p  prev=%p\n",
               i,
               (void*)curr,
               curr->size,
               curr->free,
               (void*)curr->next,
               (void*)curr->prev);
        curr = curr->next;
        i++;
    }
    printf("-----------------\n");
}

int main() {
    printf("=== Coalescing Tests ===\n");

    // Allocate 3 adjacent blocks
    void* a = my_malloc(32);
    void* b = my_malloc(32);
    void* c = my_malloc(32);

    printf("Allocated:\n");
    printf("  a = %p\n", a);
    printf("  b = %p\n", b);
    printf("  c = %p\n", c);

    print_heap();

    // Free middle block first
    printf("\nFreeing b...\n");
    my_free(b);
    print_heap();

    // Free block before it -> should merge (a + b)
    printf("\nFreeing a (should merge with b)...\n");
    my_free(a);
    print_heap();

    // Current free_head should now be the merged a+b block
    block_header* head = free_head;

    printf("\nAfter freeing a and b:\n");
    printf("  Free list head size = %zu\n", head->size);
    size_t expected_ab = 32 + 32 + sizeof(block_header);
    printf("  Expected merged size ~ %zu\n", expected_ab);

    // Now free C -> should merge ((a+b) + c)
    printf("\nFreeing c (should merge with merged block)...\n");
    my_free(c);
    print_heap();

    // Re-check free_head
    head = free_head;

    printf("\nFinal merged block size = %zu\n", head->size);
    size_t expected_abc = 32 + 32 + 32 + 2 * sizeof(block_header);
    printf("Expected fully merged size ~ %zu\n", expected_abc);

    if (head->size >= expected_abc) {
        printf("\nSUCCESS: Coalescing works correctly.\n");
    } else {
        printf("\nERROR: Coalescing size incorrect.\n");
    }

    printf("\n=== Coalesce Test Complete ===\n");
    return 0;
}
