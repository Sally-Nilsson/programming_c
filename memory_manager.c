#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define METADATA_LIMIT 0.2  // Max additional memory allowed for metadata (20%)

typedef struct Block {
    size_t size;         // Size of the memory block
    bool free;           // Is the block free?
    struct Block* next;  // Pointer to the next block
} Block;

static Block* free_list = NULL;  // Head of the free list
static void* memory_pool = NULL; // Pointer to allocated memory pool
static size_t pool_size = 0;     // Size of the memory pool
static size_t used_metadata = 0; // Tracks metadata usage

void mem_init(size_t size) {
    if (memory_pool) {
        printf("Memory manager already initialized!\n");
        return;
    }

    // Allocate pool and metadata separately
    pool_size = size;
    memory_pool = malloc(size);
    if (!memory_pool) {
        printf("Failed to allocate memory pool!\n");
        exit(1);
    }

    free_list = malloc(sizeof(Block));
    if (!free_list) {
        printf("Failed to allocate metadata!\n");
        free(memory_pool);
        exit(1);
    }
    used_metadata += sizeof(Block);
    
    // Initialize free block
    free_list->size = size;
    free_list->free = true;
    free_list->next = NULL;
}

void* mem_alloc(size_t size) {
    if (size == 0) {
        Block* curr = free_list;
        while (curr) {
            if (curr->free) {
                return memory_pool + (curr - free_list) * sizeof(Block);
            }
            curr = curr->next;
        }
        return NULL; // No free block found
    }

    Block* curr = free_list;
    Block* prev = NULL;

    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split if there's excess space
            if (curr->size > size) {
                Block* new_block = malloc(sizeof(Block));
                if (!new_block) {
                    printf("Failed to allocate metadata!\n");
                    return NULL;
                }
                used_metadata += sizeof(Block);
                
                new_block->size = curr->size - size;
                new_block->free = true;
                new_block->next = curr->next;
                
                curr->size = size;
                curr->free = false;
                curr->next = new_block;
            } else {
                curr->free = false;
            }
            return memory_pool + (curr - free_list) * sizeof(Block);
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL; // No suitable block found
}

void mem_free(void* block) {
    if (!block) return;

    Block* curr = free_list;
    Block* prev = NULL;

    // Locate the block in the free list
    while (curr) {
        if ((void*)(memory_pool + (curr - free_list) * sizeof(Block)) == block) {
            curr->free = true;

            // Merge with next block if it's free
            if (curr->next && curr->next->free) {
                curr->size += sizeof(Block) + curr->next->size;
                curr->next = curr->next->next;
            }

            // Merge with previous block if it's free
            if (prev && prev->free) {
                prev->size += sizeof(Block) + curr->size;
                prev->next = curr->next;
            }

            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void* mem_resize(void* block, size_t size) {
    if (!block) return mem_alloc(size);

    Block* curr = free_list;
    while (curr) {
        if (memory_pool + (curr - free_list) * sizeof(Block) == block) {
            if (curr->size >= size) return block;
            void* new_block = mem_alloc(size);
            if (!new_block) return NULL;
            memcpy(new_block, block, curr->size);
            mem_free(block);
            used_metadata -= sizeof(Block);
            return new_block;
        }
        curr = curr->next;
    }
    return NULL;
}

void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    pool_size = 0;
    printf("Memory pool deallocated.\n");
    Block* curr = free_list;
    while (curr) {
        Block* next = curr->next;
        free(curr);
        curr = next;
    }
    printf("Metadata deallocated.\n");
    free_list = NULL;
    used_metadata = 0;
}

void mem_print_status() {
    printf("\n\nMemory Pool Size: %zu bytes\n", pool_size);
    printf("Used Metadata: %zu bytes\n", used_metadata);
    printf("Blocks:\n");
    Block* curr = free_list;
    while (curr) {
        printf("Block at %p - Size: %zu bytes - %s\n", (void*)curr, curr->size, curr->free ? "Free" : "Allocated");
        curr = curr->next;
    }
    printf("\n");
}
