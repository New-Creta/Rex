#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/texture_format.h"
#include "rex_renderer_core/resources/resource.h"

namespace rex
{
  namespace gfx
  {
    class ResourceView;

    // A base class for a 2D texture.
    // It only holds the width, height and format
    // The actual resource is implemented in a api specific derived class
    class Texture2D : public Resource
    {
    public:
      Texture2D(ResourceView* view, s32 width, s32 height, TextureFormat format);

      // Return the width of the texture
      s32 width() const;
      // Return the height of the texture
      s32 height() const;
      // Return the format of the texture
      TextureFormat format() const;
      // Return the resource view of the texture
      ResourceView* resource_view();

    private:
      s32 m_width;
      s32 m_height;
      TextureFormat m_format;
      ResourceView* m_view;
    };
  }
}