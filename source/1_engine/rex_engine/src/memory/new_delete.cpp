#include "rex_engine/memory/new_delete.h"

#include "rex_engine/memory/global_allocators/global_allocator.h"
#include "rex_std/bonus/types.h"

#include <vcruntime_new.h>

void* operator new(u64 size) // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  return rex::GlobalAllocator().allocate(static_cast<card64>(size));
}
void* operator new[](u64 size) // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  return rex::GlobalAllocator().allocate(static_cast<card64>(size));
}

void operator delete(void* ptr) noexcept           // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  rex::GlobalAllocator().deallocate(ptr, -1);     // unknown size
}
void operator delete[](void* ptr) noexcept           // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  rex::GlobalAllocator().deallocate(ptr, -1);     // unknown size
}
void operator delete(void* ptr, u64 size) noexcept // NOLINT(readability-inconsistent-declaration-parameter-name)
{
  rex::GlobalAllocator().deallocate(ptr, static_cast<card64>(size));
}
