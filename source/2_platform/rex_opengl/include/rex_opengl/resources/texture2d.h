#pragma once

#include "rex_engine/data_type.h"
#include "rex_engine/types.h"
#include "rex_opengl/resources/resource.h"

namespace rex
{
  enum class TextureStorageFormat
  {
    Unknown,

    Depth,
    DepthStencil,

    R,
    Rg,
    Rgb,
    Rgba
  };

  enum class TexturePixelFormat
  {
    Unknown,

    Depth,
    DepthStencil,

    R,
    Rg,
    Rgb,
    Rgba,

    BGR,
    BGRA,
  };

  enum class TextureWrap
  {
    Unknown,

    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
  };

  enum class TextureFilter
  {
    Unknown,

    Nearest,
    Linear
  };

  struct TextureDescription
  {
    static TextureDescription make_default_texture_description(u32 width = 1u, u32 height = 1u)
    {
      TextureDescription desc;

      desc.storage_format  = TextureStorageFormat::Rgb;
      desc.width           = width;
      desc.height          = height;
      desc.pixel_format    = TexturePixelFormat::Rgb;
      desc.pixel_data_type = DataType::Value::UnsignedInt8;
      desc.mipmap_level    = 0;
      desc.filter          = TextureFilter::Linear;
      desc.min_filter      = TextureFilter::Linear;
      desc.mag_filter      = TextureFilter::Linear;
      desc.s_wrap          = TextureWrap::Repeat;
      desc.t_wrap          = TextureWrap::Repeat;

      return desc;
    }

    TextureDescription()
        : storage_format(TextureStorageFormat::Unknown)
        , width(0)
        , height(0)
        , pixel_format(TexturePixelFormat::Unknown)
        , pixel_data_type(DataType::Value::NONE)
        , mipmap_level(0)
        , filter(TextureFilter::Unknown)
        , min_filter(TextureFilter::Unknown)
        , mag_filter(TextureFilter::Unknown)
        , s_wrap(TextureWrap::Unknown)
        , t_wrap(TextureWrap::Unknown)
    {
    }

    TextureStorageFormat storage_format;
    s32 width;
    s32 height;
    TexturePixelFormat pixel_format;
    DataType pixel_data_type;
    s32 mipmap_level;
    TextureFilter filter;
    TextureFilter min_filter;
    TextureFilter mag_filter;
    TextureWrap s_wrap;
    TextureWrap t_wrap;
  };

  class Texture2D : public Resource
  {
  public:
    Texture2D(TextureDescription desc, void* data);
    Texture2D(const Texture2D& other) = delete;
    Texture2D(Texture2D&& other) noexcept;
    ~Texture2D() override;

    Texture2D& operator=(const Texture2D& other) = delete;
    Texture2D& operator=(Texture2D&& other) noexcept;

    u32 get_resource_id() const override;

    void bind() const override;
    void unbind() const override;

    void release() override;

  private:
    u32 m_resource_id;
    TextureDescription m_description;
  };
} // namespace rex