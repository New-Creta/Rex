#include "rex_memory_pch.h"

#include "blob_reader.h"

namespace rex
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        BlobReader::BlobReader(const memory::Blob& b)
            : m_blob(b)
            , m_read_offset(0)
        {
        }
        //-------------------------------------------------------------------------
        BlobReader::BlobReader(const memory::Blob& b, card64 offset)
            : m_blob(b)
            , m_read_offset(offset)
        {
        }

        //-------------------------------------------------------------------------
        memory::byte* BlobReader::read(const rtl::MemorySize& bytesToRead)
        {
            memory::byte* value = m_blob.read_bytes(bytesToRead, m_read_offset);
            m_read_offset += sizeof(bytesToRead);
            return value;
        }

        namespace reader
        {
            //-------------------------------------------------------------------------
            rex::memory::byte* read(const memory::Blob& b, const rtl::MemorySize& bytesToRead, const rtl::MemorySize& offset)
            {
                BlobReader reader(b, offset);
                return reader.read(bytesToRead);
            }
        }
    }
}