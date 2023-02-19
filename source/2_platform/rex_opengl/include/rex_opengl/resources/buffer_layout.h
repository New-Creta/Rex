#pragma once

#include "rex_engine/data_type.h"
#include "rex_engine/types.h"
#include "rex_std/vector.h"
#include "rex_std_extra/utility/yes_no.h"

DEFINE_YES_NO_ENUM(ShouldNormalize); // NOLINT(google-explicit-constructor,-warnings-as-errors)

namespace rex
{
  struct BufferElement
  {
    //-----------------------------------------------------------------------
    BufferElement(DataType::Value inType, s32 inComponentCount, ShouldNormalize inShouldNormalize = ShouldNormalize::no)
        : type(rsl::move(inType))
        , component_count(inComponentCount)
        , normalized(inShouldNormalize)
    {
    }

    DataType type;
    s32 component_count;
    bool normalized;
  };

  bool operator==(const BufferElement& element1, const BufferElement& element2);
  bool operator!=(const BufferElement& element1, const BufferElement& element2);

  using BufferElements = rsl::vector<BufferElement>;

  class BufferLayout
  {
  public:
    using BufferLayoutIt             = BufferElements::iterator;
    using ConstBufferLayoutIt        = BufferElements::const_iterator;
    using ReverseBufferLayoutIt      = BufferElements::reverse_iterator;
    using ConstReverseBufferLayoutIt = BufferElements::const_reverse_iterator;

  public:
    explicit BufferLayout(BufferElements elements);

    bool empty() const;
    s32 size() const;

    const BufferElements& get_buffer_elements() const;

  public:
    BufferLayoutIt begin()
    {
      return m_elements.begin();
    }
    ConstBufferLayoutIt begin() const
    {
      return m_elements.begin();
    }
    ConstBufferLayoutIt cbegin() const
    {
      return begin();
    }
    BufferLayoutIt end()
    {
      return m_elements.end();
    }
    ConstBufferLayoutIt end() const
    {
      return m_elements.end();
    }
    ConstBufferLayoutIt cend() const
    {
      return end();
    }

    ReverseBufferLayoutIt rbegin()
    {
      return m_elements.rbegin();
    }
    ConstReverseBufferLayoutIt rbegin() const
    {
      return m_elements.rbegin();
    }
    ConstReverseBufferLayoutIt crbegin() const
    {
      return m_elements.rbegin();
    }
    ReverseBufferLayoutIt rend()
    {
      return m_elements.rend();
    }
    ConstReverseBufferLayoutIt rend() const
    {
      return m_elements.rend();
    }
    ConstReverseBufferLayoutIt crend() const
    {
      return m_elements.crend();
    }

  private:
    BufferElements m_elements;
  };

  bool operator==(const BufferLayout& layout1, const BufferLayout& layout2);
  bool operator!=(const BufferLayout& layout1, const BufferLayout& layout2);

  class BufferLayoutBuilder
  {
  public:
    BufferLayoutBuilder();

    BufferLayout& add_buffer_element(DataType::Value inType, ShouldNormalize inShouldNormalize = ShouldNormalize::no);
    BufferLayout& build();

  private:
    BufferLayout m_layout;
  };
} // namespace rex