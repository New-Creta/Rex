#pragma once

#include "rex_renderer_core/resources/buffer_desc.h"
#include "rex_renderer_core/resources/buffer_layout.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/resources/compile_shader.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/link_shader.h"
#include "rex_renderer_core/resources/load_shader.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"

#include "rex_renderer_core/resource_management/resource_slot.h"

namespace rex
{
  namespace rhi
  {
    ResourceHash hash_resource_desc(const BufferDesc& desc);
    ResourceHash hash_resource_desc(const renderer::VertexBufferDesc& desc);
    ResourceHash hash_resource_desc(const renderer::IndexBufferDesc& desc);
    ResourceHash hash_resource_desc(const renderer::ConstantBufferDesc& desc);
    ResourceHash hash_resource_desc(const ClearStateDesc& desc);
    ResourceHash hash_resource_desc(const CompileShaderDesc& desc);
    ResourceHash hash_resource_desc(const InputLayoutDesc& desc);
    ResourceHash hash_resource_desc(const LinkShaderDesc& desc);
    ResourceHash hash_resource_desc(const LoadShaderDesc& desc);
    ResourceHash hash_resource_desc(const RasterStateDesc& desc);
    ResourceHash hash_resource_desc(const PipelineStateDesc& desc);
  }
}