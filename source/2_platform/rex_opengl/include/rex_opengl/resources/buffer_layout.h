#pragma once

#include "rex_engine/data_type.h"
#include "rex_engine/types.h"

#include "rex_std/vector.h"

namespace rex
{
    struct BufferElement
    {
        //-----------------------------------------------------------------------
        BufferElement(DataType inType, u64 inStride, u32 inComponentCount, bool inShouldNormalize = false)
            :type(inType)
            ,stride(inStride)
            ,component_count(inComponentCount)
            ,normalized(inShouldNormalize)
        {}

        DataType type;
        u64 stride;
        u32 component_count;
        bool normalized;
    };

    using BufferElements = rsl::vector<BufferElement>;

    class BufferLayout
    {
    public:
        using BufferLayoutIt = BufferElements::iterator;
        using ConstBufferLayoutIt = BufferElements::const_iterator;
        using ReverseBufferLayoutIt = BufferElements::reverse_iterator;
        using ConstReverseBufferLayoutIt = BufferElements::const_reverse_iterator;

        inline BufferLayoutIt                 begin() { return m_elements.begin(); }
        inline ConstBufferLayoutIt            begin() const { return m_elements.begin(); }
        inline ConstBufferLayoutIt            cbegin() const { return begin(); }
        inline BufferLayoutIt                 end() { return m_elements.end(); }
        inline ConstBufferLayoutIt            end() const { return m_elements.end(); }
        inline ConstBufferLayoutIt            cend() const { return end(); }

        inline ReverseBufferLayoutIt          rbegin() { return m_elements.rbegin(); }
        inline ConstReverseBufferLayoutIt     rbegin() const { return m_elements.rbegin(); }
        inline ConstReverseBufferLayoutIt     crbegin() const { return m_elements.rbegin(); }
        inline ReverseBufferLayoutIt          rend() { return m_elements.rend(); }
        inline ConstReverseBufferLayoutIt     rend() const { return m_elements.rend(); }
        inline ConstReverseBufferLayoutIt     crend() const { return m_elements.crend(); }

    public:
        BufferLayout(const BufferLayout& other);
        BufferLayout(BufferLayout&& other) noexcept;

        BufferLayout& operator=(const BufferLayout& other);
        BufferLayout& operator=(BufferLayout&& other) noexcept;

    public:
        BufferLayout(const BufferElements& elements);

        bool empty() const;
        s32 size() const;

        const BufferElements& get_buffer_elements() const;

    private:
        BufferElements m_elements;
    };

    bool operator==(const BufferLayout& layout1, const BufferLayout& layout2);
    bool operator!=(const BufferLayout& layout1, const BufferLayout& layout2);

    class BufferLayoutBuilder
    {
    public:
        BufferLayoutBuilder();

        BufferLayout& add_buffer_element(DataType inType, bool inShouldNormalize = false);
        BufferLayout& build();

    private:
        BufferLayout m_layout;
    };
}