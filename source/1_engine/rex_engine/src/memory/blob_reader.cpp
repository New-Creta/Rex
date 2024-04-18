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
    void BlobReader::skip(rsl::memory_size amount)
    {
      m_read_offset += amount;
    }
  }   // namespace memory
} // namespace rex