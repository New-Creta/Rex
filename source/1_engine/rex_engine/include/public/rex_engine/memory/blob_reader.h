#pragma once

#include "rex_engine/memory/blob_view.h" // IWYU pragma: keep
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {
    class BlobReader
    {
    public:
      // Create a blob reader, looking at a blob view, starting from the first byte
      explicit BlobReader(memory::BlobView b);
      // Create a blob reader, looking at a blob view, starting from the offset
      BlobReader(memory::BlobView b, rsl::memory_size offset);

      // Read data from the current offset and reinterpret is it as T and return it
      template <typename T>
      T read();

      // Read a number of bytes out of the blob and store it in dst
      void read(void* dst, rsl::memory_size size);

      // Skip a certain amount of bytes
      // it returns the read offset after the skip has been applied
      s32 skip(rsl::memory_size amount);

      // Return the current read offset
      s32 read_offset() const;

    private:
      memory::BlobView m_blob;
      rsl::memory_size m_read_offset;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    T rex::memory::BlobReader::read()
    {
      T value = m_blob.read<T>(m_read_offset);
      m_read_offset += sizeof(T);
      return value;
    }
  }   // namespace memory
} // namespace rex