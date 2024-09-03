#include "rex_engine/memory/blob_reader.h"

namespace rex
{
  namespace memory
  {
    //-------------------------------------------------------------------------
    BlobReader::BlobReader(BlobView b)
        : BlobReader(b, 0_bytes)
    {
    }
    //-------------------------------------------------------------------------
    BlobReader::BlobReader(BlobView b, rsl::memory_size offset)
        : m_blob(b)
        , m_read_offset(offset)
    {
    }

    //-------------------------------------------------------------------------
    void BlobReader::read(void* dst, rsl::memory_size size)
    {
      m_blob.read_bytes(dst, size, m_read_offset);
      m_read_offset += size;
    }

    //-------------------------------------------------------------------------
    s32 BlobReader::skip(rsl::memory_size amount)
    {
      m_read_offset += amount;
      return static_cast<s32>(m_read_offset);
    }

    //-------------------------------------------------------------------------
    s32 BlobReader::read_offset() const
    {
      return static_cast<s32>(m_read_offset);
    }

  }   // namespace memory
} // namespace rex