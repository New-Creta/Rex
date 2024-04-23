#pragma once

#include "rex_std/memory.h"

#include "rex_directx/resources/texture.h"

struct ImGuiContext;

namespace rex
{
  namespace renderer
  {
    class Device;
    class ShaderResourceView;
    class RootSignature;
    class PipelineStateObject;
    class RenderTarget;
    class CommandList;

    class ImGUIRenderer
    {
    public:
      /**
       * Begin a new ImGui frame. Do this before calling any ImGui functions that modifies ImGui's render context.
       */
      void new_frame();
      /**
       * Render ImgGui to the given render target.
       */
      void draw(rsl::shared_ptr<renderer::CommandList> commandList, const renderer::RenderTarget& renderTarget);

    protected:
      friend struct rsl::default_delete<ImGUIRenderer>;

      ImGUIRenderer(renderer::Device& device, void* hwnd, const renderer::RenderTarget& renderTarget);
      virtual ~ImGUIRenderer();

    private:
      /**
       * Destroy ImGui context.
       */
      void destroy();

    private:
      renderer::Device& m_device;
      ImGuiContext* m_imgui;
      rsl::shared_ptr<renderer::Texture> m_font_texture;
      rsl::shared_ptr<renderer::ShaderResourceView> m_font_srv;
      rsl::shared_ptr<renderer::RootSignature> m_root_signature;
      rsl::shared_ptr<renderer::PipelineStateObject> m_pipeline_state_object;
    };
  } // namespace renderer
} // namespace rex