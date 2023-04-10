#include "rex_opengl/resources/texture2d.h"

#include "rex_opengl/log.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

namespace rex 
{
  namespace texture2d
  {
    //-------------------------------------------------------------------------
    GLint texture_wrapping(const TextureWrap& wrap)
    {
      switch(wrap)
      {
        case TextureWrap::Repeat: return GL_REPEAT;
        case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;

        case TextureWrap::Unknown: break;
      }

      REX_ERROR(LogOpenGL, "Unknown \"Texture Wrap\", returning \"GL_REPEAT\"");
      return GL_REPEAT;
    }

    //-------------------------------------------------------------------------
    GLint texture_filter(const TextureFilter& filter)
    {
      switch(filter)
      {
        case TextureFilter::Linear: return GL_LINEAR;
        case TextureFilter::Nearest: return GL_NEAREST;

        case TextureFilter::Unknown: break;
      }

      REX_ERROR(LogOpenGL, "Unknown \"Texture Filter\", returning \"GL_LINEAR\"");
      return GL_LINEAR;
    }

    //-------------------------------------------------------------------------
    GLint texture_min_filter(const TextureFilter& filter)
    {
      switch(filter)
      {
        case TextureFilter::Linear: return GL_LINEAR_MIPMAP_LINEAR;
        case TextureFilter::Nearest: return GL_LINEAR_MIPMAP_NEAREST;

        case TextureFilter::Unknown: break;
      }

      REX_ERROR(LogOpenGL, "Unknown \"Texture Filter\", returning \"GL_LINEAR_MIPMAP_LINEAR\"");
      return GL_LINEAR_MIPMAP_LINEAR;
    }

    //-------------------------------------------------------------------------
    GLint texture_internal_format(const TextureStorageFormat& format)
    {
      switch(format)
      {
        case TextureStorageFormat::R: return GL_RED;
        case TextureStorageFormat::Rg: return GL_RG;
        case TextureStorageFormat::Rgb: return GL_RGB;
        case TextureStorageFormat::Rgba: return GL_RGBA;
        case TextureStorageFormat::Depth: return GL_DEPTH_COMPONENT;
        case TextureStorageFormat::DepthStencil: return GL_DEPTH_STENCIL;

        case TextureStorageFormat::Unknown: break;
      }

      REX_ERROR(LogOpenGL, "Unknown \"TextureStorageFormat\", returning \"GL_RGB\"");
      return GL_RGB;
    }

    //-------------------------------------------------------------------------
    GLint texture_pixel_format(const TexturePixelFormat& format)
    {
      switch(format)
      {
        case TexturePixelFormat::Depth: return GL_DEPTH_COMPONENT;

        case TexturePixelFormat::DepthStencil: return GL_DEPTH_STENCIL;

        case TexturePixelFormat::R: return GL_RED;
        case TexturePixelFormat::Rg: return GL_RG;
        case TexturePixelFormat::Rgb: return GL_RGB;
        case TexturePixelFormat::Rgba: return GL_RGBA;

        case TexturePixelFormat::BGR: return GL_BGR;
        case TexturePixelFormat::BGRA: return GL_BGRA;

        case TexturePixelFormat::Unknown: break;
      }

      REX_ERROR(LogOpenGL, "Unknown \"Texture Pixel Format\", returning \"GL_RGB\"");
      return GL_RGB;
    }

    //-------------------------------------------------------------------------
    GLint texture_pixel_type(const DataType& type)
    {
      switch(type.get())
      {
        case DataType::Value::UnsignedInt8: return GL_UNSIGNED_BYTE;

        case DataType::Value::None:
        case DataType::Value::Int8:
        case DataType::Value::Int16:
        case DataType::Value::Int32:
        case DataType::Value::Int64:
        case DataType::Value::UnsignedInt16:
        case DataType::Value::UnsignedInt32:
        case DataType::Value::UnsignedInt64:
        case DataType::Value::Float32:
        case DataType::Value::Float64:
        case DataType::Value::Mat3:
        case DataType::Value::Mat4:
        case DataType::Value::Vec2:
        case DataType::Value::Vec3:
        case DataType::Value::Vec4:
        case DataType::Value::Ivec2:
        case DataType::Value::Ivec3:
        case DataType::Value::Ivec4:
        case DataType::Value::Uvec2:
        case DataType::Value::Uvec3:
        case DataType::Value::Uvec4:
        case DataType::Value::Bool:
        case DataType::Value::Sampler2D:
        case DataType::Value::Sampler3D:
        case DataType::Value::SamplerCube:
        case DataType::Value::String:
          // Nothing to implement
          break;
      }

      REX_ERROR(LogOpenGL, "Unknown \"Data Type\", returning \"GL_UNSIGNED_BYTE\"");
      return GL_UNSIGNED_BYTE;
    }
  } // namespace texture2d

  //-------------------------------------------------------------------------
  Texture2D::Texture2D(TextureDescription desc, void* data)
      : m_resource_id(0)
      , m_description(rsl::move(desc))
  {
    glGenTextures(1, &m_resource_id);
    glBindTexture(GL_TEXTURE_2D, m_resource_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture2d::texture_wrapping(m_description.s_wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture2d::texture_wrapping(m_description.t_wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture2d::texture_min_filter(m_description.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture2d::texture_filter(m_description.filter));

    REX_ASSERT_X(data != nullptr, "data for texture is nullptr");

    glTexImage2D(GL_TEXTURE_2D, m_description.mipmap_level, texture2d::texture_internal_format(m_description.storage_format), m_description.width, m_description.height, 0, texture2d::texture_pixel_format(m_description.pixel_format),
                 texture2d::texture_pixel_type(m_description.pixel_data_type), data);

    glGenerateMipmap(GL_TEXTURE_2D);
  }

  //-------------------------------------------------------------------------
  Texture2D::Texture2D(Texture2D&& other) noexcept
      : m_resource_id(rsl::exchange(other.m_resource_id, 0))
      , m_description(rsl::exchange(other.m_description, {}))
  {
  }

  //-------------------------------------------------------------------------
  Texture2D::~Texture2D()
  {
    release();
  }

  //-------------------------------------------------------------------------
  Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
  {
    REX_ASSERT_X(this != &other, "can't move to yourself");

    m_resource_id = rsl::exchange(other.m_resource_id, 0);
    m_description = rsl::exchange(other.m_description, {});

    return *this;
  }

  //-------------------------------------------------------------------------
  u32 Texture2D::resource_id() const
  {
    return m_resource_id;
  }

  //-------------------------------------------------------------------------
  void Texture2D::bind() const
  {
    REX_ASSERT_X(m_resource_id != 0, "Binding an invalid resource id");
    glBindTexture(GL_TEXTURE_2D, m_resource_id);
  }

  //-------------------------------------------------------------------------
  void Texture2D::unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  //-------------------------------------------------------------------------
  void Texture2D::release()
  {
    glDeleteTextures(1, &m_resource_id);

    m_description = {};
  }

} // namespace rex 