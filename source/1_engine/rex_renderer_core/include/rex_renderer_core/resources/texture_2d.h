#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/texture_format.h"
#include "rex_renderer_core/resources/resource.h"

namespace rex
{
  namespace gfx
  {
    // A base class for a 2D texture.
    // It only holds the width, height and format
    // The actual resource is implemented in a api specific derived class
    class Texture2D : public Resource
    {
    public:
      Texture2D(s32 width, s32 height, TextureFormat format);

      s32 width() const;
      s32 height() const;
      TextureFormat format() const;

    private:
      s32 m_width;
      s32 m_height;
      TextureFormat m_format;
    };
  }
}