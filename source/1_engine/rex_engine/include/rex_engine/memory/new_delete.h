#pragma once

#include "rex_engine/types.h"

void operator delete(void* ptr) noexcept;                  // NOLINT(misc-new-delete-overloads, readability-inconsistent-declaration-parameter-name, readability-redundant-declaration)
void operator delete(void* ptr, size_t /*size*/) noexcept; // NOLINT(misc-new-delete-overloads, readability-inconsistent-declaration-parameter-name, readability-redundant-declaration)

void* operator new(u64 size); // NOLINT(readability-redundant-declaration)