#include "rex_memory_pch.h"

#include "blob_writer.h"

namespace rex
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        BlobWriter::BlobWriter(memory::Blob& b)
            : m_blob(b)
            , m_write_offset(0)
        {
        }
        //-------------------------------------------------------------------------
        BlobWriter::BlobWriter(memory::Blob& b, card64 offset)
            : m_blob(b)
            , m_write_offset(offset)
        {
        }

        //-------------------------------------------------------------------------
        void BlobWriter::write(const void* inData, const rtl::MemorySize& inSize)
        {
            m_blob.write(inData, inSize, m_write_offset);
            m_write_offset += inSize;
        }

        //-------------------------------------------------------------------------
        card64 BlobWriter::get_write_offset() const
        {
            return m_write_offset;
        }

        namespace writer
        {
            //-------------------------------------------------------------------------
            void write(memory::Blob& b, const void* inData, const rtl::MemorySize& inSize)
            {
                BlobWriter writer(b);
                writer.write(inData, inSize);
            }

            //-------------------------------------------------------------------------
            void write(memory::Blob& b, const void* inData, const rtl::MemorySize& inSize, const rtl::MemorySize& inOffset)
            {
                BlobWriter writer(b, inOffset);
                writer.write(inData, inSize);
            }
        }
    }
}