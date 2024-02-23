#include "rex_engine/memory/blob.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/memory/blob_writer.h"
#include "rex_std/bonus/memory/stack_allocator.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {
    //-------------------------------------------------------------------------
    void Blob::copy(const Blob& src, Blob& dst)
    {
      if(src.size() == 0)
      {
        REX_WARN(LogEngine, "size of src == 0, nothing to copy ...");

        return;
      }

      REX_ASSERT_X(dst.size() != 0, "dst blob size is 0");
      REX_ASSERT_X(dst.size() >= src.size(), "dst size is smaller than src size");

      rsl::memcpy(dst.m_data.get(), src.m_data.get(), src.size());
    }
    //-------------------------------------------------------------------------
    void Blob::copy(const Blob& src, void* dst)
    {
      if(src.size() == 0)
      {
        REX_WARN(LogEngine, "size of src == 0, nothing to copy ...");

        return;
      }

      REX_ASSERT_X(dst != nullptr, "dst is nullptr");

      rsl::memcpy(dst, src.m_data.get(), src.size());
    }
    //-------------------------------------------------------------------------
    void Blob::copy(void* src, const rsl::memory_size& size, Blob& dst)
    {
      REX_ASSERT_X(src != nullptr, "src is nullptr");
      REX_ASSERT_X(size != 0, "size is 0");

      REX_ASSERT_X(dst.size() != 0, "dst size is 0");
      REX_ASSERT_X(dst.size() >= size, "dst size is not big enough to receive copy");

      rsl::memcpy(dst.m_data.get(), src, size);
    }

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
    Blob::Blob(rsl::unique_array<rsl::byte> data)
        : m_data(rsl::exchange(data, nullptr))
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
    void Blob::allocate(const rsl::memory_size& inSize)
    {
      release();

      if(inSize == 0)
      {
        REX_WARN(LogEngine, "allocation of size equal to 0");

        return;
      }

      m_data = rsl::make_unique<rsl::byte[]>(static_cast<card32>(inSize)); // NOLINT(modernize-avoid-c-arrays)
    }
    //-------------------------------------------------------------------------
    void Blob::release()
    {
      m_data.reset();
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
    rsl::byte* Blob::read_bytes(rsl::byte* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset)
    {
      REX_ASSERT_X(inOffset + inSize <= size(), "amount to read out of bounds");

      rsl::memcpy(dst, static_cast<rsl::byte*>(m_data.get()) + inOffset, inSize);
      return dst;
    }

    //-------------------------------------------------------------------------
    const rsl::byte* Blob::read_bytes(rsl::byte* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset) const
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
    rsl::byte* Blob::data()
    {
      return m_data.get();
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

    //-------------------------------------------------------------------------
    Blob make_blob(const rsl::byte* inData, const rsl::memory_size& inSize)
    {
      Blob blob;

      blob.allocate(inSize);
      blob.zero_initialize();

      if(inData != nullptr)
      {
        writer::write(blob, inData, inSize);
      }

      return blob;
    }
  } // namespace memory
} // namespace rex