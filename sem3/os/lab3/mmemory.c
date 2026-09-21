#include "mmemory.h"
#include <time.h>

int generate_random(int max)
{
    return rand() % max + 1;
}

void init_memory_manager(MemoryManager *manager, size_t total_size)
{
    manager->start_addr = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (manager->start_addr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    manager->total_size = total_size;
    manager->head = (MemoryBlock *)manager->start_addr;
    manager->head->size = total_size - sizeof(MemoryBlock);
    manager->head->next = NULL;
    manager->head->isFree = true;
    srand(time(NULL));
}

void run_fragmentation(MemoryManager *manager)
{
    MemoryBlock *current = manager->head;
    while (current->size >= manager->total_size / 10 + sizeof(MemoryBlock))
    {
        int fragmentation_size = generate_random(manager->total_size / 10);
        MemoryBlock *new_block = (MemoryBlock *)((char *)current + fragmentation_size + sizeof(MemoryBlock));
        new_block->size = current->size - fragmentation_size - sizeof(MemoryBlock);
        current->next = new_block;
        current->size = fragmentation_size;
        new_block->isFree = true;
        current = current->next;
    }
    current->next = NULL;
}
void *allocate_memory(MemoryManager *manager, size_t size)
{
    MemoryBlock *current = manager->head;
    while (current != NULL)
    {
        if (current->isFree && current->size >= size)
        {
            current->isFree = false;
            return (char *)current + sizeof(MemoryBlock);
        }
        current = current->next;
    }
    return NULL;
}
void free_memory(void *ptr)
{
    MemoryBlock *block = (MemoryBlock *)((char *)ptr - sizeof(MemoryBlock));
    block->isFree = true;
}
void clear(MemoryManager *manager)
{
    munmap(manager->start_addr, manager->total_size);
    manager->head = NULL;
}

char *safe_read(void *dest, size_t size)
{
    MemoryBlock *block = (MemoryBlock *)((char *)dest - sizeof(MemoryBlock));
    if (block->isFree)
    {
        printf("Error: Attempt to read from a free block\n");
        return NULL;
    }
    if (block->size < size)
    {
        printf("Error: Attempt to read beyond block size\n");
        return NULL;
    }
    return (char *)block + sizeof(MemoryBlock);
}
void safe_write(void *dest, size_t size, char *info)
{
    MemoryBlock *block = (MemoryBlock *)((char *)dest - sizeof(MemoryBlock));
    if (block->isFree)
    {
        printf("Error: Attempt to write to a free block\n");
        return;
    }
    if (block->size < size)
    {
        printf("Error: Attempt to write beyond block size\n");
        return;
    }
    char *data = (char *)block + sizeof(MemoryBlock);
    strncpy(data, info, size - 1);
    data[size - 1] = '\0';
}
