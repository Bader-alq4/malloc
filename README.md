# Custom Memory Allocator (C)

## Overview
This project is a custom implementation of `malloc()` and `free()` written in **C**.  
It was built to understand how dynamic memory allocation works and how a heap allocator manages memory internally.

The allocator manages its own heap using `sbrk()`, tracks metadata for each block, and supports memory reuse through splitting and coalescing.

---

## How It Works
Memory is divided into blocks with the following layout:

| block_header | user_memory |



- The **block header** stores metadata such as size, allocation state, and links to other blocks.
- The **user memory** is the portion returned to the caller.

The allocator maintains:
- A **heap list** (all blocks in memory order)
- A **free list** (only free blocks)

---

## Allocation & Freeing
- `my_malloc()` searches the free list first, splits blocks when possible, and only grows the heap with `sbrk()` when needed.
- `my_free()` marks blocks as free and coalesces adjacent free blocks to reduce fragmentation.

---

## Key Features
- Manual heap management using `sbrk()`
- Block splitting and coalescing
- Separate heap and free linked lists
- Aligned allocations
- Static library output (`liballocator.a`)
- Built with a custom Makefile

---

## Tech Stack
- **Language:** C  
- **Build:** Makefile  
- **Debugging:** GDB, Valgrind  
- **OS Interface:** `sbrk()`
