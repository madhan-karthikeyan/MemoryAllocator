# Custom Memory Allocator

This project implements a custom memory allocator in C using `malloc`, `free`, `calloc`, and `realloc` functionalities. The implementation uses a linked list to manage memory blocks and employs thread-safety with `pthread_mutex`.

## Key Features
- Custom implementation of `malloc`, `free`, `calloc`, and `realloc`.
- Thread-safe memory operations using `pthread_mutex`.
- Efficient reuse of freed memory blocks.
- Debugging utility `print_mem_heap()` to visualize the state of the memory heap.

## Working

### 1. Memory Allocation (`malloc`)
- Allocates a memory block of the requested size.
- Checks for free blocks in the linked list using `get_free_block()`.
- If a suitable block is found, it is reused; otherwise, a new block is created using `sbrk()`.
- The allocated memory block is aligned to 16 bytes for efficiency.

### 2. Memory Deallocation (`free`)
- Marks the specified block as free for reuse.
- If the block is at the end of the heap, it is released using `sbrk()`.
- Updates the linked list to maintain integrity.

### 3. Contiguous Memory Allocation (`calloc`)
- Allocates a block large enough to hold `num * nsize` bytes.
- Initializes the allocated memory to zero.

### 4. Resizing Memory Block (`realloc`)
- Resizes an existing memory block to the specified size.
- If the existing block is large enough, it is reused.
- Otherwise, a new block is allocated, and the contents of the old block are copied to it.

### 5. Debugging Utility (`print_mem_heap`)
- Prints the state of the memory heap.
- Displays the address, size, free status, and next pointer of each block in the heap.

## Methods Used

### `header_t *get_free_block(size_t size)`
- Traverses the linked list to find a free block with sufficient size.

### `void *malloc(size_t size)`
- Allocates memory by either reusing a free block or creating a new one.

### `void free(void *block)`
- Frees the specified memory block and updates the linked list or releases memory back to the system.

### `void *calloc(size_t num, size_t nsize)`
- Allocates and initializes memory to zero for an array of `num` elements of size `nsize`.

### `void *realloc(void *block, size_t size)`
- Resizes a memory block, copying the data to a new block if necessary.

### `void print_mem_heap()`
- Prints the current state of the memory heap for debugging purposes.

## Example Usage
```c
#include "custom_malloc.h"

int main() {
    int *arr = (int *)malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++)
        arr[i] = i;

    print_mem_heap();

    free(arr);

    print_mem_heap();

    return 0;
}
```

## Thread Safety
All memory operations are protected by a `pthread_mutex` to ensure thread safety in a multi-threaded environment.

## Limitations
- The implementation does not handle fragmentation optimally.
- It relies on `sbrk()` which is less flexible compared to modern memory management functions like `mmap()`.

## Debugging
Use the `print_mem_heap()` function to visualize the state of the memory heap. This is helpful for debugging and understanding how memory is being managed.

## Build and Run
1. Compile the program using a C compiler (e.g., `gcc`).
2. Include this file in your project and link the `pthread` library.
   ```sh
   gcc -o custom_malloc main.c -lpthread
   ```
3. Run the executable.

---

This project provides a foundation for understanding memory management concepts and can be extended to handle more complex memory allocation scenarios.
