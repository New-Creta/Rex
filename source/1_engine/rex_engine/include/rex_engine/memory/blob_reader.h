#pragma once

#include "rex_engine/memory/blob.h" // IWYU pragma: keep
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {
    class BlobReader
    {
    public:
      explicit BlobReader(const memory::Blob& b);
      BlobReader(const memory::Blob& b, const rsl::memory_size& offset);

      template <typename T>
      T read();

      rsl::byte* read(const rsl::memory_size& bytesToRead);

    private:
      const memory::Blob* m_blob;
      rsl::memory_size m_read_offset;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    T rex::memory::BlobReader::read()
    {
      T value = m_blob->read<T>(m_read_offset);
      m_read_offset += sizeof(T);
      return value;
    }

    namespace reader
    {
      //-------------------------------------------------------------------------
      template <typename T>
      T read(const memory::Blob& b)
      {
        BlobReader reader(b);
        return reader.read<T>();
      }
      //-------------------------------------------------------------------------
      template <typename T>
      T read(const memory::Blob& b, const rsl::memory_size& offset)
      {
        BlobReader reader(b, offset);
        return reader.read<T>();
      }

      //-------------------------------------------------------------------------
      rsl::byte* read(const memory::Blob& b, const rsl::memory_size& bytesToRead, const rsl::memory_size& offset);
    } // namespace reader
  }   // namespace memory
} // namespace rex