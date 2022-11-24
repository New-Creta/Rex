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
            static void copy(void* src, const rsl::MemorySize& size, Blob& dst);

            Blob();
            Blob(const Blob& other) = delete;
            Blob(Blob&& other) noexcept;
            Blob(rsl::unique_ptr<rsl::byte> data, const rsl::MemorySize& dataSize);

            Blob& operator=(const Blob& other) = delete;
            Blob& operator=(Blob&& other) noexcept;

            explicit operator bool() const;

            rsl::byte& operator[](int32 index);
            const rsl::byte& operator[](int32 index) const;

            void allocate(const rsl::MemorySize& inSize);
            void release();
            void zero_initialize();

            rsl::byte* data();
            const rsl::byte* data() const;

            const rsl::MemorySize& size() const;

            template <typename T>
            T* data_as();
            template <typename T>
            const T* data_as() const;

        private:
            friend class BlobWriter;
            friend class BlobReader;

            template <typename T>
            T& read(const rsl::MemorySize& offset = 0);
            template <typename T>
            const T& read(const rsl::MemorySize& offset = 0) const;

            rsl::byte* read_bytes(rsl::byte* dst, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset);
            const rsl::byte* read_bytes(rsl::byte* dst, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset) const;

            void write(const void* inData, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset = 0_bytes);

            rsl::unique_ptr<rsl::byte> m_data;
            rsl::MemorySize m_size;
        };

        //-------------------------------------------------------------------------
        Blob make_blob(const rsl::byte* inData, const rsl::MemorySize& inSize);

        //-------------------------------------------------------------------------
        template <typename T>
        T& Blob::read(const rsl::MemorySize& offset /*= 0*/)
        {
            return *(T*)m_data.get() + offset;
        }

        //-------------------------------------------------------------------------
        template <typename T>
        const T& Blob::read(const rsl::MemorySize& offset) const
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