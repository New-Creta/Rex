#include "rex_engine/memory/new_delete.h"

#include <cstdlib>

void operator delete(void* ptr) noexcept // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  free(ptr); // NOLINT(cppcoreguidelines-no-malloc)
}
void operator delete(void* ptr, size_t /*size*/) noexcept // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  free(ptr); // NOLINT(cppcoreguidelines-no-malloc)
}

void* operator new(u64 size) // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  return malloc(size); // NOLINT(cppcoreguidelines-no-malloc)
}