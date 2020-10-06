#pragma once

#include "shaderdatatype.h"

namespace rex
{
    namespace graphics
    {
        struct BufferElement
        {
            std::string     name;
            ShaderDataType  type;
            unsigned int    size;
            unsigned int    offset;
            bool            normalized;

            //-------------------------------------------------------------------------
            BufferElement()
                :name("")
                ,type(rex::graphics::ShaderDataType::NONE)
                ,size(0)
                ,offset(0)
                ,normalized(false)
            {}
            //-------------------------------------------------------------------------
            BufferElement(ShaderDataType type, const std::string& elementName, bool shouldNormalize = false)
                :name(elementName), type(type), size(shaderDataTypeSize(type)), offset(0), normalized(shouldNormalize)
            {}
        };

        class BufferLayout
        {
        public:
            //-------------------------------------------------------------------------
            BufferLayout()
                :m_stride(0)
            {}
            //-------------------------------------------------------------------------
            BufferLayout(const std::initializer_list<BufferElement>& elements)
                : m_elements(elements)
                , m_stride(0)
            {
                calculateOffsetsAndStride();
            }

            inline unsigned int                         getStride() const { return m_stride; }
            inline const std::vector<BufferElement>&    getElements() const { return m_elements; }

            std::vector<BufferElement>::iterator        begin() { return m_elements.begin(); }
            std::vector<BufferElement>::iterator        end() { return m_elements.end(); }
            std::vector<BufferElement>::const_iterator  begin() const { return m_elements.cbegin(); }
            std::vector<BufferElement>::const_iterator  end() const { return m_elements.cend(); }

        private:
            //-------------------------------------------------------------------------
            void calculateOffsetsAndStride()
            {
                unsigned int offset = 0;
                m_stride = 0;

                for (auto& element : m_elements)
                {
                    element.offset = offset;
                    offset += element.size;
                    m_stride += element.size;
                }
            }

        private:
            std::vector<BufferElement> m_elements;

            unsigned int m_stride;
        };
    }
}