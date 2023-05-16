#include "rex_engine/memory/new_delete.h"

#include <vcruntime_new.h>                       // for operator delete

#include "rex_engine/memory/global_allocator.h"  // for global_allocator
#include "rex_std/bonus/types.h"                 // for card64

void* operator new(u64 size) // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  return rex::global_allocator().allocate(static_cast<card64>(size));
}

void operator delete(void* ptr) noexcept // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  rex::global_allocator().deallocate(ptr, -1); // unknown size
}
void operator delete(void* ptr, u64 size) noexcept // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  rex::global_allocator().deallocate(ptr, static_cast<card64>(size));
}
