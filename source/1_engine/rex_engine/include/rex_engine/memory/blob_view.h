#pragma once

#include "rex_std/bonus/memory.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {

    class Blob;

    // Similar design of string_view -> string.
    // A blob view has a const view over a blob
    // It doesn't own the blob but can provide an interface into
    // the data of a blob

    class BlobView
    {
    public:
      BlobView();
      BlobView(const Blob& blob);
      BlobView(const void* data, rsl::memory_size size);

      const rsl::byte* data() const;
      rsl::memory_size size() const;

      template <typename T>
      const T* data_as() const
      {
        return reinterpret_cast<T*>(data());
      }

    private:
      const rsl::byte* m_data;
      rsl::memory_size m_size;
    };
  } // namespace memory
} // namespace rex