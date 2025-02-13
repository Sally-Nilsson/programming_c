#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct Block {
    size_t size;         // Size of the memory block
    bool free;           // Is the block free
    void* pointer_memory; // Pointer to the memory block
    struct Block* next;  // Pointer to the next block
} Block;

static Block* block_list = NULL;  // Head of the free list
static void* memory_pool = NULL; // Pointer to allocated memory pool

void mem_init(size_t size) {
    if (memory_pool) {
        printf("Memory manager already initialized!\n");
        return;
    }

    memory_pool = malloc(size);
    if (!memory_pool) {
        printf("Failed to allocate memory pool!\n");
        return;
    }

    block_list = malloc(sizeof(Block));
    if (!block_list) {
        printf("Failed to allocate metadata!\n");
        free(memory_pool);
        return;
    }
    
    // Initialize first block
    block_list->size = size;
    block_list->free = true;
    block_list->pointer_memory = memory_pool;
    block_list->next = NULL;
}

void* mem_alloc(size_t size) {
    // If the size is 0, find the first free block
    if (size == 0) {
        Block* curr = block_list;
        while (curr) {
            if (curr->free) {
                return curr->pointer_memory;
            }
            curr = curr->next;
        }
        return NULL; // No free block found
    }

    Block* curr = block_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split if there's excess space
            if (curr->size > size) {
                Block* new_block = malloc(sizeof(Block));
                if (!new_block) {
                    printf("Failed to allocate metadata!\n");
                    return NULL;
                }
                
                new_block->size = curr->size - size;
                new_block->pointer_memory = curr->pointer_memory + size;
                new_block->free = true;
                new_block->next = curr->next;
                
                curr->size = size;
                curr->free = false;
                curr->next = new_block;
            } else {
                curr->free = false;
            }
            return curr->pointer_memory;
        }
        curr = curr->next;
    }
    return NULL; // No suitable block found
}

void mem_free(void* block) {
    if (!block) return;

    Block* curr = block_list;
    Block* prev = NULL;

    // Locate the block in the block list
    while (curr) {
        if (curr->pointer_memory == block) {
            curr->free = true;
            printf("Block at %p freed.\n", block);

            Block* next = curr->next;

            // Merge with next block if it's free
            if (next && next->free) {
                curr->size += next->size;
                curr->next = next->next;
                free(next);
                printf("Merged with next block.\n");
            }

            // Merge with previous block if it's free
            if (prev && prev->free) {
                prev->size += curr->size;
                prev->next = curr->next;
                free(curr);
                printf("Merged with previous block.\n");
            }

            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void* mem_resize(void* block, size_t size) {
    if (!block || size == 0) return NULL;

    Block* curr = block_list;

    // Find the block that is going to be resized in the block list
    while (curr) {
        if (curr->pointer_memory == block) {
            printf("Block found.\n");
            break;
        }
        curr = curr->next;
    }

    // If shrinking the block
    if (curr->size > size) {
        printf("Shrinking block.\n");
        Block* new_block = malloc(sizeof(Block));
        if (!new_block) return NULL;
        
        new_block->size = curr->size - size;
        new_block->pointer_memory = curr->pointer_memory + size;
        new_block->free = true;
        new_block->next = curr->next;
        
        curr->size = size;
        curr->next = new_block;
        return curr->pointer_memory;
    }

    // If expanding the block
    if (curr->next && curr->next->free && curr->next->size + curr->size >= size) {
        printf("Expaning block.\n");
        // Check next block if it's free and has enough space
        Block* next_block = curr->next;
        curr->size = next_block->size;
        curr->next = next_block->next;
        free(next_block);
        return curr->pointer_memory;
    }
    // Resize to the same size
    return curr->pointer_memory;
}

void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    printf("Memory pool deallocated.\n");

    Block* curr = block_list;
    while (curr) {
        Block* next = curr->next;
        if (curr)
            free(curr);
        curr = next;
    }
    printf("Metadata deallocated.\n");
    block_list = NULL;
}

void mem_print_status() {
    // Prints the block list and their status
    printf("Blocks:\n");
    Block* curr = block_list;
    while (curr) {
        printf("Block at %p - Size: %zu bytes - %s\n", (void*)curr, curr->size, curr->free ? "Free" : "Allocated");
        curr = curr->next;
    }
    printf("\n");
}
