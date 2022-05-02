#pragma once

namespace rex
{
    namespace memory
    {
        class Blob
        {
        public:
            static void copy(const Blob& src, Blob& dst);
            static void copy(const Blob& src, void* dst);
            static void copy(void* src, const rtl::MemorySize& size, Blob& dst);

            Blob();
            Blob(const Blob& other) = delete;
            Blob(Blob&& other) noexcept;
            Blob(rtl::UniquePtr<memory::byte> data, const rtl::MemorySize& dataSize);

            Blob& operator=(const Blob& other) = delete;
            Blob& operator=(Blob&& other) noexcept;

            explicit operator bool() const;

            memory::byte& operator[](int32 index);
            const memory::byte& operator[](int32 index) const;

            void allocate(const rtl::MemorySize& inSize);
            void release();
            void zero_initialize();

            memory::byte* data();
            const memory::byte* data() const;

            const rtl::MemorySize& size() const;

            template <typename T>
            T* data_as();
            template <typename T>
            const T* data_as() const;

        private:
            friend class BlobWriter;
            friend class BlobReader;

            template <typename T>
            T& read(const rtl::MemorySize& offset = 0);
            template <typename T>
            const T& read(const rtl::MemorySize& offset = 0) const;

            memory::byte* read_bytes(memory::byte* dst, const rtl::MemorySize& inSize, const rtl::MemorySize& inOffset);
            const memory::byte* read_bytes(memory::byte* dst, const rtl::MemorySize& inSize, const rtl::MemorySize& inOffset) const;

            void write(const void* inData, const rtl::MemorySize& inSize, const rtl::MemorySize& inOffset = 0_bytes);

            rtl::UniquePtr<memory::byte> m_data;
            rtl::MemorySize m_size;
        };

        //-------------------------------------------------------------------------
        Blob make_blob(const memory::byte* inData, const rtl::MemorySize& inSize);

        //-------------------------------------------------------------------------
        template <typename T>
        T& Blob::read(const rtl::MemorySize& offset /*= 0*/)
        {
            return *(T*)m_data.get() + offset;
        }

        //-------------------------------------------------------------------------
        template <typename T>
        const T& Blob::read(const rtl::MemorySize& offset) const
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
    }
}