#pragma once

#include "resources/resource.h"

#include "data_type.h"

namespace rex
{
    enum class TextureStorageFormat
    {
        DEPTH,
        DEPTH_STENCIL,

        R,
        RG,
        RGB,
        RGBA
    };

    enum class TexturePixelFormat
    {
        DEPTH,
        DEPTH_STENCIL,

        R,
        RG,
        RGB,
        RGBA,

        BGR,
        BGRA,
    };

    enum class TextureWrap
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class TextureFilter
    {
        NEAREST,
        LINEAR
    };

    struct TextureDescription
    {
        TextureDescription()
            :storage_format(TextureStorageFormat::RGB)
            ,width(1)
            ,height(1)
            ,pixel_format(TexturePixelFormat::RGB)
            ,pixel_data_type(DataType::Value::UNSIGNED_INT8)
            ,mipmap_level(0)
            ,filter(TextureFilter::LINEAR)
            ,min_filter(TextureFilter::LINEAR)
            ,mag_filter(TextureFilter::LINEAR)
            ,s_wrap(TextureWrap::REPEAT)
            ,t_wrap(TextureWrap::REPEAT)
        {}

        TextureStorageFormat storage_format;
        u32 width;
        u32 height;
        TexturePixelFormat pixel_format;
        DataType pixel_data_type;
        u32 mipmap_level;
        TextureFilter filter;
        TextureFilter min_filter;
        TextureFilter mag_filter;
        TextureWrap s_wrap;
        TextureWrap t_wrap;
        
    };

    class Texture2D : public Resource
    {
    public:
        Texture2D(const TextureDescription& desc, void* data);
        ~Texture2D() override;

        u32 get_resource_id() const override;

        void bind() const override;
        void unbind() const override;

        void release() override;

    private:
        u32 m_resource_id;
        TextureDescription m_description;
    };
}