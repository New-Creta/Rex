#include "rex_engine/memory/blob_view.h"

namespace rex
{
  BlobView::BlobView()
    : m_data(nullptr)
    , m_size(0)
  {}

  BlobView::BlobView(const Blob& blob)
    : m_data(blob.data())
    , m_size(blob.size())
  {}

  const rsl::byte* BlobView::data() const
  {
    return m_data;
  }

  rsl::memory_size BlobView::size() const
  {
    return m_size;
  }
}