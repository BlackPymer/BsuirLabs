#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
typedef struct MemoryBlock
{
    size_t size;
    struct MemoryBlock *next;
    bool isFree;
} MemoryBlock;
typedef struct
{
    void *start_addr;
    size_t total_size;
    MemoryBlock *head;
} MemoryManager;

int generate_random(int max);
void init_memory_manager(MemoryManager *manager, size_t total_size);
void run_fragmentation(MemoryManager *manager);
void *allocate_memory(MemoryManager *manager, size_t size);
void free_memory(void *ptr);
void clear(MemoryManager *manager);

char *safe_read(void *dest, size_t size);
void safe_write(void *dest, size_t size, char *info);