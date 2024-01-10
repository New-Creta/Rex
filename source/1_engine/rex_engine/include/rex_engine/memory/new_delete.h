#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/attributes.h"

void operator delete(void* ptr) noexcept;           // NOLINT(misc-new-delete-overloads, readability-inconsistent-declaration-parameter-name, readability-redundant-declaration)
void operator delete(void* ptr, u64 size) noexcept; // NOLINT(misc-new-delete-overloads, readability-inconsistent-declaration-parameter-name, readability-redundant-declaration)

REX_NO_DISCARD void* operator new(u64 size); // NOLINT(readability-redundant-declaration)