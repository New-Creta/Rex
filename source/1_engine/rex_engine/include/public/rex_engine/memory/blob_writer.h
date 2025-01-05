#pragma once

#include "rex_engine/memory/blob.h" // IWYU pragma: keep
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/types.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  namespace memory
  {
    class BlobWriter
    {
    public:
      explicit BlobWriter(memory::Blob& b);
      BlobWriter(memory::Blob& b, rsl::memory_size offset);

      template <typename T>
      void write(const T& data);
      void write(const void* inData, rsl::memory_size inSize);

      card64 write_offset() const;

    private:
      memory::Blob* m_blob;
      rsl::memory_size m_write_offset;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    void rex::memory::BlobWriter::write(const T& data)
    {
      m_blob->write(&data, sizeof(T), m_write_offset);
      m_write_offset += sizeof(T);
    }

    namespace writer
    {
      //-------------------------------------------------------------------------
      template <typename T>
      void write(memory::Blob& b, const T& data)
      {
        BlobWriter writer(b);

        writer.write<T>(data);
      }

      void write(memory::Blob& b, const void* inData, rsl::memory_size inSize);
      void write(memory::Blob& b, const void* inData, rsl::memory_size inSize, rsl::memory_size inOffset);
    } // namespace writer
  }   // namespace memory
} // namespace rex