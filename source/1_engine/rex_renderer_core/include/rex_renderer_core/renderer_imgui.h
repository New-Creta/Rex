#pragma once

namespace rex
{
  namespace renderer
  {
    namespace imgui
    {
      bool initialize();
      void shutdown();

      void begin();
      void render();
      void end();
    } // namespace imgui
  }   // namespace renderer
} // namespace rex