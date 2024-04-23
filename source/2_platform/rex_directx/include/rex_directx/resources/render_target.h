#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include "rex_renderer_core/iresource.h"

#include "rex_std/vector.h"
#include "rex_std/memory.h"

#include <DirectXMath.h>

namespace rex
{
    namespace renderer
    {
        // Don't use scoped enums to avoid the explicit cast required to use these as 
        // array indices.
        enum AttachmentPoint
        {
            Color0,
            Color1,
            Color2,
            Color3,
            Color4,
            Color5,
            Color6,
            Color7,

            DepthStencil,
            
            NumAttachmentPoints
        };

        class Texture;

        class RenderTarget : public IResource
        {
        public:
            RESOURCE_CLASS_TYPE(RenderTarget);

            // Create an empty render target.
            RenderTarget();

            RenderTarget( const RenderTarget& copy ) = default;
            RenderTarget( RenderTarget&& copy ) = default;

            RenderTarget& operator=( const RenderTarget& other ) = default;
            RenderTarget& operator=( RenderTarget&& other ) = default;

            /**
             * Attach a texture to a given attachment point.
             *
             * @param attachmentPoint The point to attach the texture to.
             * @param [texture] Optional texture to bind to the render target. Specify nullptr to remove the texture.
             */
            void attach_texture( AttachmentPoint attachmentPoint, const rsl::shared_ptr<Texture>& texture );

            // Resize all of the textures associated with the render target.
            void                resize(DirectX::XMUINT2 size);
            void                resize(u32 width, u32 height);
            DirectX::XMUINT2    size() const;
            u32                 width() const;
            u32                 height() const;

            // Get a viewport for this render target.
            // The scale and bias parameters can be used to specify a split-screen
            // viewport (the bias parameter is normalized in the range [0...1]).
            // By default, a fullscreen viewport is returned.
            D3D12_VIEWPORT viewport(DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }, DirectX::XMFLOAT2 bias = { 0.0f, 0.0f }, float minDepth = 0.0f, float maxDepth = 1.0f) const;

            const rsl::shared_ptr<Texture>& texture( AttachmentPoint attachmentPoint ) const;
            // Get a list of the textures attached to the render target.
            // This method is primarily used by the CommandList when binding the
            // render target to the output merger stage of the rendering pipeline.
            const rsl::vector<rsl::shared_ptr<Texture>>& textures() const;

            // Get the render target formats of the textures currently 
            // attached to this render target object.
            // This is needed to configure the Pipeline state object.
            D3D12_RT_FORMAT_ARRAY render_target_formats() const;

            // Get the format of the attached depth/stencil buffer.
            DXGI_FORMAT depth_stencil_format() const;

            // Get the sample description of the render target.
            DXGI_SAMPLE_DESC sample_desc() const;

            // Reset all textures
            void reset();

        private:
            using render_target_list = rsl::vector<rsl::shared_ptr<Texture>>;

            render_target_list m_textures;
            DirectX::XMUINT2 m_size;
        };
    }
}