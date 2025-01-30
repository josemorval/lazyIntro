#ifndef LIBNEM_HEADER_H
#define LIBMEM_HEADER_H

static char static_memory[32 * 1024 * 1024];
static unsigned int static_memory_pointer = 0;

#define allocation(var,type, ...) var=(type*)allocate_object(sizeof(type)); *var = type(__VA_ARGS__);
void* allocate_object(unsigned int size)
{
    void* ptr = &(static_memory[static_memory_pointer]);
    static_memory_pointer += size;
    return ptr;
}

#endif