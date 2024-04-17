#include "rex_engine/memory/blob.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/casting.h"
#include "rex_engine/memory/blob_writer.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/memory/stack_allocator.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {
    //-------------------------------------------------------------------------
    Blob::Blob()
        : m_data(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    Blob::Blob(Blob&& other) noexcept
        : m_data(rsl::exchange(other.m_data, nullptr))
    {
    }
    //-------------------------------------------------------------------------
    Blob::Blob(rsl::unique_array<rsl::byte>&& data)
        : m_data(rsl::move(data))
    {
    }

    Blob::Blob(void* data, rsl::memory_size size)
        : m_data((rsl::byte*)data, narrow_cast<s32>(size.size_in_bytes())) // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
    {
    }

    //-------------------------------------------------------------------------
    Blob::~Blob() = default;

    //-------------------------------------------------------------------------
    Blob& Blob::operator=(Blob&& other) noexcept
    {
      // Guard self assignment
      REX_ASSERT_X(this != &other, "can't move to yourself");

      m_data = rsl::exchange(other.m_data, nullptr);

      return *this;
    }

    //-------------------------------------------------------------------------
    Blob::operator bool() const
    {
      return m_data.get() != nullptr;
    }

    //-------------------------------------------------------------------------
    rsl::byte& Blob::operator[](int32 index)
    {
      return m_data.get()[index];
    }
    //-------------------------------------------------------------------------
    const rsl::byte& Blob::operator[](int32 index) const
    {
      return (static_cast<const rsl::byte*>(m_data.get()))[index];
    }

    //-------------------------------------------------------------------------
    void Blob::zero_initialize()
    {
      if(m_data)
      {
        rsl::memset(m_data.get(), 0, size());
      }
    }

    //-------------------------------------------------------------------------
    void* Blob::read_bytes(void* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset) const
    {
      REX_ASSERT_X(inOffset + inSize <= size(), "amount of read out of bounds");

      rsl::memcpy(dst, static_cast<const rsl::byte*>(m_data.get()) + inOffset, inSize);
      return dst;
    }

    //-------------------------------------------------------------------------
    void Blob::write(const void* inData, const rsl::memory_size& inSize, const rsl::memory_size& inOffset)
    {
      REX_ASSERT_X(inOffset + inSize <= size(), "amount for write out of bounds");

      rsl::memcpy(static_cast<rsl::byte*>(m_data.get()) + inOffset, inData, inSize);
    }

    //-------------------------------------------------------------------------
    const rsl::byte* Blob::data() const
    {
      return m_data.get();
    }

    //-------------------------------------------------------------------------
    rsl::memory_size Blob::size() const
    {
      return rsl::memory_size(m_data.count());
    }
  } // namespace memory
} // namespace rex