#pragma once

#include "rex_memory/blob.h"

namespace rex
{
    namespace memory
    {
        class BlobWriter
        {
        public:
            BlobWriter(memory::Blob& b);
            BlobWriter(memory::Blob& b, card64 offset);

            template <typename T>
            void write(const T& data);
            void write(const void* inData, const rsl::MemorySize& inSize);

            card64 write_offset() const;

        private:
            memory::Blob& m_blob;
            rsl::MemorySize m_write_offset;
        };

        //-------------------------------------------------------------------------
        template <typename T>
        void rex::memory::BlobWriter::write(const T& data)
        {
            m_blob.write(&data, sizeof(T), m_write_offset);
            m_write_offset += sizeof(T);
        }

        namespace writer
        {
            //-------------------------------------------------------------------------
            template <typename T>
            void write(memory::Blob& b, const T& data)
            {
                BlobWriter writer(b);

                writer.template write<T>(data);
            }

            void write(memory::Blob& b, const void* inData, const rsl::MemorySize& inSize);
            void write(memory::Blob& b, const void* inData, const rsl::MemorySize& inSize, const rsl::MemorySize& inOffset);
        }
    }
}