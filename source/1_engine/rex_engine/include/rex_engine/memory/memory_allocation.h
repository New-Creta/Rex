#pragma once

namespace rex
{
    void* memory_alloc(size_t size_bytes);
    void* memory_calloc(size_t count, size_t size_bytes);
    void* memory_realloc(void* mem, size_t size_bytes);
    void  memory_free(void* mem);
    void  memory_zero(void* dest, size_t size_bytes);
}