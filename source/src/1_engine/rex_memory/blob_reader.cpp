#include "rex_memory_pch.h"

#include "blob_reader.h"

namespace rex
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        BlobReader::BlobReader(const memory::Blob& b)
            : BlobReader(b, 0_bytes)
        {
        }
        //-------------------------------------------------------------------------
        BlobReader::BlobReader(const memory::Blob& b, const rtl::MemorySize& offset)
            : m_blob(b)
            , m_read_offset(offset)
        {
        }

        //-------------------------------------------------------------------------
        memory::byte* BlobReader::read(const rtl::MemorySize& bytesToRead)
        {
            memory::byte* dst = new memory::byte[bytesToRead];
            
            m_blob.read_bytes(dst, bytesToRead, m_read_offset);
            m_read_offset += bytesToRead;

            return dst;
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