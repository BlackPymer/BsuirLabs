#include "mmemory.h"
#include <stdio.h>
#include <string.h>

static int tests_passed = 0;
static int tests_failed = 0;

static void check(bool cond, const char *name)
{
    if (cond)
    {
        printf("[PASS] %s\n", name);
        tests_passed++;
    }
    else
    {
        printf("[FAIL] %s\n", name);
        tests_failed++;
    }
}

static void test_init(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    check(m.start_addr != NULL, "init: mmap ok");
    check(m.total_size == 4096, "init: total_size == 4096");
    check(m.head == (MemoryBlock *)m.start_addr, "init: head at start_addr");
    check(m.head->size == 4096 - sizeof(MemoryBlock), "init: head size correct");
    check(m.head->isFree == true, "init: head is free");
    check(m.head->next == NULL, "init: head next is NULL");
    clear(&m);
    check(m.head == NULL, "init: clear sets head NULL");
}

static void test_fragmentation(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    run_fragmentation(&m);

    size_t used = 0;
    int count = 0;
    bool all_free = true;
    MemoryBlock *b = m.head;
    while (b != NULL)
    {
        used += sizeof(MemoryBlock) + b->size;
        count++;
        if (!b->isFree)
        {
            all_free = false;
        }
        b = b->next;
    }
    check(count >= 2, "frag: more than one block");
    check(used <= m.total_size, "frag: blocks fit into mmap");
    check(all_free, "frag: all blocks free");
    clear(&m);
}

static void test_allocate(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);

    void *p = allocate_memory(&m, 16);
    check(p != NULL, "alloc: small block ok");
    check(p == (char *)m.head + sizeof(MemoryBlock), "alloc: returns data area");
    check(m.head->isFree == false, "alloc: block marked busy");

    void *q = allocate_memory(&m, 16);
    check(q == NULL, "alloc: no free block left");

    free_memory(p);
    check(m.head->isFree == true, "free: block free again");

    void *r = allocate_memory(&m, 16);
    check(r == p, "alloc: reuses freed block");
    clear(&m);
}

static void test_allocate_too_big(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    void *p = allocate_memory(&m, 6000);
    check(p == NULL, "alloc: too big returns NULL");
    clear(&m);
}

static void test_safe_read_write(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    void *p = allocate_memory(&m, 32);
    safe_write(p, 32, "hello");
    check(strcmp(safe_read(p, 32), "hello") == 0, "rw: roundtrip ok");
    clear(&m);
}

static void test_out_of_bounds(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);

    void *p = allocate_memory(&m, 16);
    check(safe_read(p, 6000) == NULL, "oob: read beyond block returns NULL");
    safe_write(p, 6000, "too long");
    check(safe_read(p, 32) != NULL, "oob: write refused, data intact");

    free_memory(p);
    check(safe_read(p, 16) == NULL, "oob: read from free block returns NULL");
    clear(&m);
}

int main(void)
{
    test_init();
    test_fragmentation();
    test_allocate();
    test_allocate_too_big();
    test_safe_read_write();
    test_out_of_bounds();

    printf("Tests passed: %d, failed: %d\n", tests_passed, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}