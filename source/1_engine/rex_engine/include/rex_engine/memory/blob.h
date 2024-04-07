#pragma once

#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/types.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {
    class Blob
    {
    public:
      static void copy(const Blob& src, Blob& dst);
      static void copy(const Blob& src, void* dst);
      static void copy(void* src, const rsl::memory_size& size, Blob& dst);

      Blob();
      Blob(const Blob& other) = delete;
      Blob(Blob&& other) noexcept;
      explicit Blob(rsl::unique_array<rsl::byte> data);
      template <typename T>
      explicit Blob(rsl::unique_array<T>&& data)
          : m_data()
      {
        // "release" sets internal count to 0
        // so we need to make sure we cache this data before actually calling release.
        const s32 total_size = data.byte_size();
        m_data         = rsl::unique_array<rsl::byte>((rsl::byte*)data.release(), total_size); // NOLINT(google-readability-casting)
      }
      Blob(void* data, rsl::memory_size size);
      ~Blob();

      Blob& operator=(const Blob& other) = delete;
      Blob& operator=(Blob&& other) noexcept;

      explicit operator bool() const;

      rsl::byte& operator[](int32 index);
      const rsl::byte& operator[](int32 index) const;

      void allocate(const rsl::memory_size& inSize);
      void release();
      void zero_initialize();

      rsl::byte* data();
      const rsl::byte* data() const;

      rsl::memory_size size() const;

      template <typename T>
      T* data_as();
      template <typename T>
      const T* data_as() const;

      template <typename T>
      T& read(const rsl::memory_size& offset = 0_bytes);
      template <typename T>
      const T& read(const rsl::memory_size& offset = 0_bytes) const;

      rsl::byte* read_bytes(rsl::byte* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset);
      const rsl::byte* read_bytes(rsl::byte* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset) const;

      void write(const void* inData, const rsl::memory_size& inSize, const rsl::memory_size& inOffset = 0_bytes);

    private:
      rsl::unique_array<rsl::byte> m_data;
    };

    //-------------------------------------------------------------------------
    Blob make_blob(const rsl::byte* inData, const rsl::memory_size& inSize);
    //-------------------------------------------------------------------------
    template <typename T>
    Blob make_blob(const T* data, int32 num)
    {
      return make_blob(reinterpret_cast<const rsl::byte*>(data), rsl::memory_size(sizeof(T) * num));
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T& Blob::read(const rsl::memory_size& offset /*= 0*/)
    {
      T* data = reinterpret_cast<T*>(m_data.get() + offset);
      return *data;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    const T& Blob::read(const rsl::memory_size& offset) const
    {
      return *(T*)(m_data.get() + offset);
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T* Blob::data_as()
    {
      return reinterpret_cast<T*>(m_data.get());
    }
    //-------------------------------------------------------------------------
    template <typename T>
    const T* Blob::data_as() const
    {
      return reinterpret_cast<const T*>(m_data.get());
    }
  } // namespace memory
} // namespace rex