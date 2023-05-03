#include "rex_engine/frameinfo/frameinfo.h"

namespace rex
{
  FrameInfo::FrameInfo()
      : m_delta_time()
      , m_fps()
      , m_index(-1)
  {
  }

  void FrameInfo::update()
  {
    m_delta_time.update();
    m_fps.update();
    m_index++;
  }

  const DeltaTime& FrameInfo::delta_time() const
  {
    return m_delta_time;
  }
  const FPS& FrameInfo::fps() const
  {
    return m_fps;
  }
  card32 FrameInfo::index() const
  {
    return m_index;
  }
} // namespace rex