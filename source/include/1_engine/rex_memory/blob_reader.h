#pragma once

#include "blob.h"

namespace rex
{
    namespace memory
    {
        class BlobReader
        {
        public:
            BlobReader(const memory::Blob& b);
            BlobReader(const memory::Blob& b, const rtl::MemorySize& offset);

            template <typename T>
            T read();

            memory::byte* read(const rtl::MemorySize& bytesToRead);

        private:
            const memory::Blob& m_blob;
            rtl::MemorySize m_read_offset;
        };

        //-------------------------------------------------------------------------
        template <typename T>
        T rex::memory::BlobReader::read()
        {
            T value = m_blob.template read<T>(m_read_offset);
            m_read_offset += sizeof(T);
            return value;
        }

        namespace reader
        {
            //-------------------------------------------------------------------------
            template <typename T>
            T read(const memory::Blob& b)
            {
                BlobReader reader(b);
                return reader.template read<T>();
            }
            //-------------------------------------------------------------------------
            template <typename T>
            T read(const memory::Blob& b, const rtl::MemorySize& offset)
            {
                BlobReader reader(b, offset);
                return reader.template read<T>();
            }

            //-------------------------------------------------------------------------
            memory::byte* read(const memory::Blob& b, const rtl::MemorySize& bytesToRead, const rtl::MemorySize& offset);
        }
    }
}