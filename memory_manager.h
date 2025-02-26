#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct block {
    int is_allocated;
    int size;
};

extern void* memory_pool;
extern int pool_size;

void mem_init(size_t size);
void* mem_alloc(size_t size);
void mem_free(void* block);
void* mem_resize(void* block, size_t size);
void mem_deinit();
void mem_print_status();

#endif /* MEMORY_MANAGER_H */
