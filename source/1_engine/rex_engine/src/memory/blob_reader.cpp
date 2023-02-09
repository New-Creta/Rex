

#include "rex_engine/memory/blob_reader.h"

namespace rex
{
  namespace memory
  {
    //-------------------------------------------------------------------------
    BlobReader::BlobReader(const memory::Blob& b)
        : BlobReader(b, 0_bytes)
    {
    }
    //-------------------------------------------------------------------------
    BlobReader::BlobReader(const memory::Blob& b, const rsl::memory_size& offset)
        : m_blob(&b)
        , m_read_offset(offset)
    {
    }

    //-------------------------------------------------------------------------
    rsl::byte* BlobReader::read(const rsl::memory_size& bytesToRead)
    {
      rsl::byte* dst = new rsl::byte[bytesToRead];

      m_blob->read_bytes(dst, bytesToRead, m_read_offset);
      m_read_offset += bytesToRead;

      return dst;
    }

    namespace reader
    {
      //-------------------------------------------------------------------------
      rsl::byte* read(const memory::Blob& b, const rsl::memory_size& bytesToRead, const rsl::memory_size& offset)
      {
        BlobReader reader(b, offset);
        return reader.read(bytesToRead);
      }
    } // namespace reader
  }   // namespace memory
} // namespace rex