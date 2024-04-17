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
      // Initialize a blob with no underlying buffer
      Blob();
      Blob(const Blob& other) = delete;
      // Initialize a blob taken the underlying buffer from the other buffer
      // leaving the other buffer with no underlying buffer afterwards
      Blob(Blob&& other) noexcept;
      // Initialize a blob from a byte array.
      // This simply assigns the underlying buffer to this byte array
      explicit Blob(rsl::unique_array<rsl::byte>&& data);
      // Initialize a blob from a array
      // This simply assigns the underlying buffer to this byte array
      template <typename T>
      explicit Blob(rsl::unique_array<T>&& data)
          : m_data()
      {
        // "release" sets internal count to 0
        // so we need to make sure we cache this data before actually calling release.
        const s32 total_size = data.byte_size();
        m_data         = rsl::unique_array<rsl::byte>((rsl::byte*)data.release(), total_size); // NOLINT(google-readability-casting)
      }
      // Initialize a blob from a pointer with size.
      // The blob takes ownership of the passed in pointer
      Blob(void* data, rsl::memory_size size);
      // Release the underlying pointer, giving the memory back to where it came from
      ~Blob();

      Blob& operator=(const Blob& other) = delete;
      // Release the underlying buffer and takes it from the other blob
      // The other blob doesn't hold a buffer anymore afterwards
      Blob& operator=(Blob&& other) noexcept;

      // Returns true if the blob holds a buffer
      // Returns false if it doesn't
      explicit operator bool() const;

      // Access into the underlying buffer by byte offset.
      rsl::byte& operator[](int32 index);
      const rsl::byte& operator[](int32 index) const;

      // Zero out the underlying buffer, setting all bytes to 0
      void zero_initialize();

      // Returns a const access to the Blob's underlying buffer
      const rsl::byte* data() const;
      // Returns the size of the underlying buffer
      rsl::memory_size size() const;

      // Returns the address of the underlying buffer casted into a specific type
      template <typename T>
      const T* data_as() const;

      // Read bytes from the underlying buffer at a certain offset and cast them to a type
      template <typename T>
      const T& read(const rsl::memory_size& offset = 0_bytes) const;

      // Read x amount of bytes from the underlying buffer at a certain offset and copy them into a desintation
      void* read_bytes(void* dst, const rsl::memory_size& inSize, const rsl::memory_size& inOffset) const;

      // Write x amount of bytes into the underlying buffer, starting from a certain offset
      void write(const void* inData, const rsl::memory_size& inSize, const rsl::memory_size& inOffset = 0_bytes);

    private:
      rsl::unique_array<rsl::byte> m_data;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    const T& Blob::read(const rsl::memory_size& offset) const
    {
      return *(T*)(m_data.get() + offset);
    }

    //-------------------------------------------------------------------------
    template <typename T>
    const T* Blob::data_as() const
    {
      return reinterpret_cast<const T*>(m_data.get());
    }
  } // namespace memory
} // namespace rex