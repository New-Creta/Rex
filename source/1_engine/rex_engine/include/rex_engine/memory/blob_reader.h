#pragma once

#include "rex_engine/memory/blob.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
    namespace memory
    {
        class BlobReader
        {
        public:
            BlobReader(const memory::Blob& b);
            BlobReader(const memory::Blob& b, const rsl::MemorySize& offset);

            template <typename T>
            T read();

            rsl::byte* read(const rsl::MemorySize& bytesToRead);

        private:
            const memory::Blob& m_blob;
            rsl::MemorySize m_read_offset;
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
            T read(const memory::Blob& b, const rsl::MemorySize& offset)
            {
                BlobReader reader(b, offset);
                return reader.template read<T>();
            }

            //-------------------------------------------------------------------------
            rsl::byte* read(const memory::Blob& b, const rsl::MemorySize& bytesToRead, const rsl::MemorySize& offset);
        }
    }
}