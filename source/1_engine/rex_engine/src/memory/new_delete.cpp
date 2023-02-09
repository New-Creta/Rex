#include "rex_engine/memory/new_delete.h"

#include <cstdlib>

void operator delete(void* ptr)
{
  free(ptr);
}
void operator delete(void* ptr, size_t /*size*/)
{
  free(ptr);
}

void* operator new(u64 size)
{
  return malloc(size);
}