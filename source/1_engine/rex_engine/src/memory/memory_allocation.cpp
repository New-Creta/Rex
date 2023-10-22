#include "rex_engine/memory/memory_allocation.h"

#include <stdio.h>
#include <stdlib.h>

#include <rex_std/memory.h>

namespace rex
{
    //-------------------------------------------------------------------------
    void* memory_alloc(size_t size_bytes)
    {
        return malloc(size_bytes);
    }
    //-------------------------------------------------------------------------
    void* memory_calloc(size_t count, size_t size_bytes)
    {
        return calloc(count, size_bytes);
    }
    //-------------------------------------------------------------------------
    void* memory_realloc(void* mem, size_t size_bytes)
    {
        return realloc(mem, size_bytes);
    }
    //-------------------------------------------------------------------------
    void memory_free(void* mem)
    {
        free(mem);
    }
    //-------------------------------------------------------------------------
    void memory_zero(void* dest, size_t size_bytes)
    {
        rsl::memset(dest, 0x00, size_bytes);
    }
}