#include "rex_renderer_core/viewport.h"

namespace rex
{
  //-------------------------------------------------------------------------
  Viewport::Viewport()
      : m_top_left_x(0.0f)
      , m_top_left_y(0.0f)

      , m_width(0.0f)
      , m_height(0.0f)

      , m_min_depth(0.0f)
      , m_max_depth(1.0f)
  {
  }
  //-------------------------------------------------------------------------
  Viewport::Viewport(const ViewportCreationInfo& info)
      : m_top_left_x(info.top_left_x)
      , m_top_left_y(info.top_left_y)

      , m_width(info.width)
      , m_height(info.height)

      , m_min_depth(info.min_depth)
      , m_max_depth(info.max_depth)
  {
  }
  //-------------------------------------------------------------------------
  Viewport::~Viewport() = default;

  //-------------------------------------------------------------------------
  float Viewport::top_left_x() const
  {
    return m_top_left_x;
  }
  //-------------------------------------------------------------------------
  float Viewport::top_left_y() const
  {
    return m_top_left_y;
  }

  //-------------------------------------------------------------------------
  float Viewport::width() const
  {
    return m_width;
  }
  //-------------------------------------------------------------------------
  float Viewport::height() const
  {
    return m_height;
  }

  //-------------------------------------------------------------------------
  float Viewport::min_depth() const
  {
    return m_min_depth;
  }
  //-------------------------------------------------------------------------
  float Viewport::max_depth() const
  {
    return m_max_depth;
  }
} // namespace rex