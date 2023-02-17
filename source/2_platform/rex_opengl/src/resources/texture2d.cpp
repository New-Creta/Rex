#include "rex_opengl/resources/texture2d.h"

#include "rex_engine/diagnostics/logging.h"

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
    GLenum get_texture_wrapping(const TextureWrap& wrap)
    {
      switch (wrap)
      {
      case TextureWrap::REPEAT:
        return GL_REPEAT;
      case TextureWrap::MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;
      case TextureWrap::CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
      case TextureWrap::CLAMP_TO_BORDER:
        return GL_CLAMP_TO_BORDER;
      }

      REX_ERROR("Unknown \"Texture Wrap\", returning \"GL_REPEAT\"");
      return GL_REPEAT;
    }

    //-------------------------------------------------------------------------
    GLenum get_texture_filter(const TextureFilter& filter)
    {
      switch (filter)
      {
      case TextureFilter::LINEAR:
        return GL_LINEAR;
      case TextureFilter::NEAREST:
        return GL_NEAREST;
      }

      REX_ERROR("Unknown \"Texture Filter\", returning \"GL_LINEAR\"");
      return GL_LINEAR;
    }

    //-------------------------------------------------------------------------
    GLenum get_texture_min_filter(const TextureFilter& filter)
    {
      switch (filter)
      {
      case TextureFilter::LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
      case TextureFilter::NEAREST:
        return GL_LINEAR_MIPMAP_NEAREST;
      }

      REX_ERROR("Unknown \"Texture Filter\", returning \"GL_LINEAR_MIPMAP_LINEAR\"");
      return GL_LINEAR_MIPMAP_LINEAR;
    }

    //-------------------------------------------------------------------------
    GLenum get_texture_internal_format(const TextureStorageFormat& format)
    {
      switch (format)
      {
      case TextureStorageFormat::R:
        return GL_RED;
      case TextureStorageFormat::RG:
        return GL_RG;
      case TextureStorageFormat::RGB:
        return GL_RGB;
      case TextureStorageFormat::RGBA:
        return GL_RGBA;
      case TextureStorageFormat::DEPTH:
        return GL_DEPTH_COMPONENT;
      case TextureStorageFormat::DEPTH_STENCIL:
        return GL_DEPTH_STENCIL;
      }

      REX_ERROR("Unknown \"TextureStorageFormat\", returning \"GL_RGB\"");
      return GL_RGB;
    }

    //-------------------------------------------------------------------------
    GLenum get_texture_pixel_format(const TexturePixelFormat& format)
    {
      switch (format)
      {
      case TexturePixelFormat::DEPTH:
        return GL_DEPTH_COMPONENT;

      case TexturePixelFormat::DEPTH_STENCIL:
        return GL_DEPTH_STENCIL;

      case TexturePixelFormat::R:
        return GL_RED;
      case TexturePixelFormat::RG:
        return GL_RG;
      case TexturePixelFormat::RGB:
        return GL_RGB;
      case TexturePixelFormat::RGBA:
        return GL_RGBA;

      case TexturePixelFormat::BGR:
        return GL_BGR;
      case TexturePixelFormat::BGRA:
        return GL_BGRA;
      }

      REX_ERROR("Unknown \"Texture Pixel Format\", returning \"GL_RGB\"");
      return GL_RGB;
    }

    //-------------------------------------------------------------------------
    GLenum get_texture_pixel_type(const DataType& type)
    {
      switch (type.get())
      {
      case DataType::Value::UNSIGNED_INT8:
        return GL_UNSIGNED_BYTE;
      }

      REX_ERROR("Unknown \"Data Type\", returning \"GL_UNSIGNED_BYTE\"");
      return GL_UNSIGNED_BYTE;
    }
  }

  //-------------------------------------------------------------------------
  Texture2D::Texture2D(const TextureDescription& desc, void* data)
    :m_resource_id(0)
    , m_description(desc)
  {
    glGenTextures(1, &m_resource_id);
    glBindTexture(GL_TEXTURE_2D, m_resource_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture2d::get_texture_wrapping(m_description.s_wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture2d::get_texture_wrapping(m_description.t_wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture2d::get_texture_min_filter(m_description.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture2d::get_texture_filter(m_description.filter));

    REX_ASSERT(data != nullptr);

    glTexImage2D(GL_TEXTURE_2D
      , m_description.mipmap_level
      , texture2d::get_texture_internal_format(m_description.storage_format)
      , m_description.width
      , m_description.height
      , 0
      , texture2d::get_texture_pixel_format(m_description.pixel_format)
      , texture2d::get_texture_pixel_type(m_description.pixel_data_type)
      , data);

    glGenerateMipmap(GL_TEXTURE_2D);
  }

  //-------------------------------------------------------------------------
  Texture2D::~Texture2D()
  {
    release();
  }

  //-------------------------------------------------------------------------
  u32 Texture2D::get_resource_id() const
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

}