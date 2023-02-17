#pragma once

#include "rex_opengl/resources/resource.h"

#include "rex_engine/data_type.h"
#include "rex_engine/types.h"

namespace rex
{
    enum class TextureStorageFormat
    {
        UNKNOWN, 

        DEPTH,
        DEPTH_STENCIL,

        R,
        RG,
        RGB,
        RGBA
    };

    enum class TexturePixelFormat
    {
        UNKNOWN, 

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
        UNKNOWN, 

        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class TextureFilter
    {
        UNKNOWN,

        NEAREST,
        LINEAR
    };

    struct TextureDescription
    {
        static TextureDescription make_default_texture_description(u32 width = 1u, u32 height = 1u)
        {
            TextureDescription desc;

            desc.storage_format = TextureStorageFormat::RGB;
            desc.width = width;
            desc.height = height;
            desc.pixel_format = TexturePixelFormat::RGB;
            desc.pixel_data_type = DataType::Value::UNSIGNED_INT8;
            desc.mipmap_level = 0;
            desc.filter = TextureFilter::LINEAR;
            desc.min_filter = TextureFilter::LINEAR;
            desc.mag_filter = TextureFilter::LINEAR;
            desc.s_wrap = TextureWrap::REPEAT;
            desc.t_wrap = TextureWrap::REPEAT;  

            return desc; 
        }

        TextureDescription()
            :storage_format(TextureStorageFormat::UNKNOWN)
            ,width(0)
            ,height(0)
            ,pixel_format(TexturePixelFormat::UNKNOWN)
            ,pixel_data_type(DataType::Value::NONE)
            ,mipmap_level(0)
            ,filter(TextureFilter::UNKNOWN)
            ,min_filter(TextureFilter::UNKNOWN)
            ,mag_filter(TextureFilter::UNKNOWN)
            ,s_wrap(TextureWrap::UNKNOWN)
            ,t_wrap(TextureWrap::UNKNOWN)
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