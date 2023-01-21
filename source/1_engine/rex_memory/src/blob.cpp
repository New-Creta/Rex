

#include "rex_memory/blob.h"
#include "rex_memory/blob_writer.h"
#include "rex_std/utility.h"
#include "rex_std/memory.h"

namespace rex
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        void Blob::copy(const Blob& src, Blob& dst)
        {
            if (src.m_size == 0)
            {
                // R_WARN("size of src == 0, nothing to copy ...");

                return;
            }

            // R_ASSERT(dst.m_size != 0)
            // R_ASSERT(dst.m_size >= src.m_size);

            rsl::memcpy(dst.m_data.get(), src.m_data.get(), src.m_size);
        }
        //-------------------------------------------------------------------------
        void Blob::copy(const Blob& src, void* dst)
        {
            if (src.m_size == 0)
            {
                // R_WARN("size of src == 0, nothing to copy ...");

                return;
            }

            // R_ASSERT(dst != nullptr);

            rsl::memcpy(dst, src.m_data.get(), src.m_size);
        }
        //-------------------------------------------------------------------------
        void Blob::copy(void* src, const rsl::MemorySize& size, Blob& dst)
        {
            // R_ASSERT(src != nullptr);
            // R_ASSERT(size != 0);

            // R_ASSERT(dst.m_size != 0)
            // R_ASSERT(dst.m_size >= size);

          rsl::memcpy(dst.m_data.get(), src, size);
        }

        //-------------------------------------------------------------------------
        Blob::Blob()
            : m_data(nullptr)
            , m_size(0_bytes)
        {
        }

        //-------------------------------------------------------------------------
        Blob::Blob(Blob&& other) noexcept 
            : m_data(rsl::exchange(other.m_data, nullptr))
            , m_size(rsl::exchange(other.m_size, 0_bytes))
        {
        }
        //-------------------------------------------------------------------------
        Blob::Blob(rsl::unique_ptr<rsl::byte> data, const rsl::MemorySize& dataSize)
            : m_data(rsl::exchange(data, nullptr))
            , m_size(dataSize)
        {
        }

        //-------------------------------------------------------------------------
        Blob& Blob::operator=(Blob&& other) noexcept
        {
            // Guard self assignment
            //R_ASSERT(this == &other);

            m_data = rsl::exchange(other.m_data, nullptr);
            m_size = rsl::exchange(other.m_size, 0_bytes);

            return *this;
        }

        //-------------------------------------------------------------------------
        Blob::operator bool() const
        {
            return m_data != nullptr;
        }

        //-------------------------------------------------------------------------
        rsl::byte& Blob::operator[](int32 index)
        {
            return m_data.get()[index];
        }
        //-------------------------------------------------------------------------
        const rsl::byte& Blob::operator[](int32 index) const
        {
            return ((rsl::byte*)m_data.get())[index];
        }

        //-------------------------------------------------------------------------
        void Blob::allocate(const rsl::MemorySize& inSize)
        {
            release();

            if (inSize == 0)
            {
                // R_WARN("allocation of size equal to 0");

                return;
            }

            m_data = rsl::unique_ptr<rsl::byte>(new rsl::byte[inSize]);
            m_size = inSize;
        }
        //-------------------------------------------------------------------------
        void Blob::release()
        {
            m_data.reset();
            m_size = 0_bytes;
        }
        //-------------------------------------------------------------------------
        void Blob::zero_initialize()
        {
            if (m_data)
            {
                rsl::memset(m_data.get(), 0, m_size);
            }
        }

        //-------------------------------------------------------------------------
        rsl::byte* Blob::read_bytes(rsl::byte* dst, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset)
        {
            // R_ASSERT_X(inOffset + inSize <= m_size, "Buffer overflow!");

            rsl::memcpy(dst, (rsl::byte*)m_data.get() + inOffset, inSize);
            return dst;
        }

        //-------------------------------------------------------------------------
        const rsl::byte* Blob::read_bytes(rsl::byte* dst, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset) const
        {
            // R_ASSERT_X(inOffset + inSize <= m_size, "Buffer overflow!");

            rsl::memcpy(dst, (rsl::byte*)m_data.get() + inOffset, inSize);
            return dst;
        }

        //-------------------------------------------------------------------------
        void Blob::write(const void* inData, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset)
        {
            // R_ASSERT_X(inOffset + inSize <= m_size, "Buffer overflow!");

            rsl::memcpy((rsl::byte*)m_data.get() + inOffset, inData, inSize);
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
        const rsl::MemorySize& Blob::size() const
        {
            return m_size;
        }

        //-------------------------------------------------------------------------
        Blob make_blob(const rsl::byte* inData, const rsl::MemorySize& inSize)
        {
            Blob blob;

            blob.allocate(inSize);
            blob.zero_initialize();

            if (inData != nullptr)
            {
                writer::write(blob, inData, inSize);
            }

            return blob;
        }
    }
}