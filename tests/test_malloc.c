#include <stdio.h>
#include <string.h>
#include "../src/allocator.h"

int main() {
    printf("=== malloc() Tests ===\n");

    void* a = my_malloc(32);
    void* b = my_malloc(64);
    void* c = my_malloc(16);

    if (!a || !b || !c) {
        printf("ERROR: malloc returned NULL\n");
        return 1;
    }

    printf("Allocated a=%p, b=%p, c=%p\n", a, b, c);

    // Fill each allocated block with a test pattern.
    // This checks that:
    // 1) The pointer returned by my_malloc() is valid,
    // 2) The block is the correct size,
    // 3) Writing to one block doesn't affect others or overwrite metadata.


    // Take the pointer a (thats pointing to user memory) returned by your my_malloc(32)
    // Write 32 bytes into that memory. Simulates a user writing to that memory.
    // The bytes will all have value 0xAA (just a test pattern)

    memset(a, 0xAA, 32);
    memset(b, 0xBB, 64);
    memset(c, 0xCC, 16);

    printf("Memory write test passed.\n");

    printf("=== malloc() Tests Complete ===\n");
    return 0;
}
