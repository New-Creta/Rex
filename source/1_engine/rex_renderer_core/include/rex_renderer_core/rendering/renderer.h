#pragma once

namespace rex
{
  // Renderer interface, very basic implementation and will likely be extended in the future
  class Renderer
  {
  public:
    virtual ~Renderer() = default;

    // Prepare the renderer for a new frame
    virtual void new_frame() = 0;

    // Render a new frame
    virtual void render() = 0;
  };
}