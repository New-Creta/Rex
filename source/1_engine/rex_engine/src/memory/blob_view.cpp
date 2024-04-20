#include "rex_engine/memory/blob_view.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace memory
  {
    //-------------------------------------------------------------------------
    BlobView::BlobView()
        : m_data(nullptr)
        , m_size(0)
    {
    }

    //-------------------------------------------------------------------------
    BlobView::BlobView(const Blob& blob)
        : m_data(blob.data())
        , m_size(blob.size())
    {
    }
    //-------------------------------------------------------------------------
    BlobView::BlobView(const void* data, rsl::memory_size size)
        : m_data((const rsl::byte*)data) // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
        , m_size(size)
    {
    }

    //-------------------------------------------------------------------------
    BlobView::operator bool() const
    {
      return m_data != nullptr;
    }

    //-------------------------------------------------------------------------
    const rsl::byte& BlobView::operator[](int32 index) const
    {
      return (static_cast<const rsl::byte*>(m_data))[index];
    }

    //-------------------------------------------------------------------------
    void* BlobView::read_bytes(void* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset) const
    {
      REX_ASSERT_X(inOffset + inSize <= size(), "amount of read out of bounds");

      rsl::memcpy(dst, static_cast<const rsl::byte*>(m_data) + inOffset, inSize);
      return dst;
    }

    //-------------------------------------------------------------------------
    const rsl::byte* BlobView::data() const
    {
      return m_data;
    }

    //-------------------------------------------------------------------------
    rsl::memory_size BlobView::size() const
    {
      return rsl::memory_size(m_size);
    }

  } // namespace memory
} // namespace rex