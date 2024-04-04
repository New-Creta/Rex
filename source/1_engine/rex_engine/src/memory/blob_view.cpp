#include "rex_engine/memory/blob_view.h"

#include "rex_engine/memory/blob.h"

namespace rex
{
  namespace memory
  {
    BlobView::BlobView()
        : m_data(nullptr)
        , m_size(0)
    {
    }

    BlobView::BlobView(const Blob& blob)
        : m_data(blob.data())
        , m_size(blob.size())
    {
    }
    BlobView::BlobView(const void* data, rsl::memory_size size)
      : m_data((const rsl::byte*)data)
      , m_size(size)
    {
    }

    const rsl::byte* BlobView::data() const
    {
      return m_data;
    }

    rsl::memory_size BlobView::size() const
    {
      return m_size;
    }
  } // namespace memory
} // namespace rex