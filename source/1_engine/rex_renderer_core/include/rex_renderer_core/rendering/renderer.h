#pragma once

namespace rex
{
  class Renderer
  {
  public:
    virtual ~Renderer() = default;

    virtual void new_frame() = 0;
    virtual void render() = 0;
  };
}