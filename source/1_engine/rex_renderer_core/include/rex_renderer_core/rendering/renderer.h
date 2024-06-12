#pragma once

namespace rex
{
  // Renderer interface, very basic implementation and will likely be extended in the future
  class Renderer
  {
  public:
    virtual ~Renderer() = default;

    virtual void new_frame() = 0;
    virtual void render() = 0;
  };
}