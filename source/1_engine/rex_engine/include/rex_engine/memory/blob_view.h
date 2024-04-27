#pragma once

#include "rex_std/bonus/memory.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace memory
  {

    class Blob;

    // Similar design of string_view -> string.
    // A blob view has a const view over a blob
    // It doesn't own the blob but can provide an interface into
    // the data of a blob

    class BlobView
    {
    public:
      BlobView();
      BlobView(const Blob& blob); // NOLINT(google-explicit-constructor)
      BlobView(const void* data, rsl::memory_size size);

      // Returns true if the blob view holds a buffer
      // Returns false if it doesn't
      explicit operator bool() const;

      // Access into the underlying buffer by byte offset.
      const rsl::byte& operator[](int32 index) const;

      // Returns a const access to the blob view's underlying buffer
      const rsl::byte* data() const;
      // Returns the size of the underlying buffer
      rsl::memory_size size() const;

      // Returns the address of the underlying buffer casted into a specific type
      template <typename T>
      const T* data_as() const;

      // Read bytes from the underlying buffer at a certain offset and cast them to a type
      template <typename T>
      const T& read(rsl::memory_size offset = 0_bytes) const;

      // Read x amount of bytes from the underlying buffer at a certain offset and copy them into a desintation
      void* read_bytes(void* dst, rsl::memory_size inSize, rsl::memory_size inOffset) const;

    private:
      const rsl::byte* m_data;
      rsl::memory_size m_size;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    const T& BlobView::read(rsl::memory_size offset) const
    {
      return *(T*)(m_data + offset);
    }

    //-------------------------------------------------------------------------
    template <typename T>
    const T* BlobView::data_as() const
    {
      return reinterpret_cast<const T*>(m_data);
    }

  } // namespace memory
} // namespace rex