#pragma once

namespace rex
{
    namespace graphics
    {
        enum class ShaderDataType
        {
            NONE = 0,
            FLOAT, 
            FLOAT2, 
            FLOAT3, 
            FLOAT4,

            MAT3, 
            MAT4, 

            INT, 
            INT2, 
            INT3, 
            INT4, 
            
            BOOL
        };

        static unsigned int shaderDataTypeSize(ShaderDataType type)
        {
            switch (type)
            {
            case ShaderDataType::FLOAT:    return 4;
            case ShaderDataType::FLOAT2:   return 4 * 2;
            case ShaderDataType::FLOAT3:   return 4 * 3;
            case ShaderDataType::FLOAT4:   return 4 * 4;
            case ShaderDataType::MAT3:     return 4 * 3 * 3;
            case ShaderDataType::MAT4:     return 4 * 4 * 4;
            case ShaderDataType::INT:      return 4;
            case ShaderDataType::INT2:     return 4 * 2;
            case ShaderDataType::INT3:     return 4 * 3;
            case ShaderDataType::INT4:     return 4 * 4;
            case ShaderDataType::BOOL:     return 1;
            }

            RX_ASSERT_X(false, "Unknown ShaderDataType!");
            return 0;
        }

        struct BufferElement
        {
            std::string     name;
            ShaderDataType  type;
            unsigned int    size;
            unsigned int    offset;
            bool            normalized;

            BufferElement() 
            {}
            BufferElement(ShaderDataType type, const std::string& elementName, bool shouldNormalize = false)
                :name(elementName), type(type), size(shaderDataTypeSize(type)), offset(0), normalized(shouldNormalize)
            {
            }

            unsigned int getComponentCount() const
            {
                switch (type)
                {
                case ShaderDataType::FLOAT:   return 1;
                case ShaderDataType::FLOAT2:  return 2;
                case ShaderDataType::FLOAT3:  return 3;
                case ShaderDataType::FLOAT4:  return 4;
                case ShaderDataType::MAT3:    return 3 * 3;
                case ShaderDataType::MAT4:    return 4 * 4;
                case ShaderDataType::INT:     return 1;
                case ShaderDataType::INT2:    return 2;
                case ShaderDataType::INT3:    return 3;
                case ShaderDataType::INT4:    return 4;
                case ShaderDataType::BOOL:    return 1;
                }

                RX_ASSERT_X(false, "Unknown ShaderDataType!");
                return 0;
            }
        };

        class BufferLayout
        {
        public:
            BufferLayout() {}

            BufferLayout(const std::initializer_list<BufferElement>& elements)
                : m_elements(elements)
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
            unsigned int m_stride = 0;
        };
    }
}