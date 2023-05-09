#pragma once

#include "rex_engine/types.h"

namespace rex
{
  struct ViewportCreationInfo
  {
    f32 top_left_x;
    f32 top_left_y;

    f32 width;
    f32 height;

    f32 min_depth;
    f32 max_depth;
  };

  class Viewport
  {
  public:
    Viewport();
    explicit Viewport(const ViewportCreationInfo& info);

    f32 top_left_x() const;
    f32 top_left_y() const;
    f32 width() const;
    f32 height() const;
    f32 min_depth() const;
    f32 max_depth() const;

  private:
    f32 m_top_left_x;
    f32 m_top_left_y;

    f32 m_width;
    f32 m_height;

    f32 m_min_depth;
    f32 m_max_depth;
  };
} // namespace rex