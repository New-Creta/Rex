#pragma once

#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/resources/compile_shader.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/link_shader.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/constant_buffer.h"

#include "rex_renderer_core/resource_management/resource_slot.h"

namespace rex
{
  namespace rhi
  {
    ResourceHash hash_resource_desc(const VertexBufferDesc& desc);
    ResourceHash hash_resource_desc(const IndexBufferDesc& desc);
    ResourceHash hash_resource_desc(const ConstantBufferDesc& desc);
    ResourceHash hash_resource_desc(const ClearStateDesc& desc);
    ResourceHash hash_resource_desc(const CompileShaderDesc& desc);
    ResourceHash hash_resource_desc(const InputLayoutDesc& desc);
    ResourceHash hash_resource_desc(const LinkShaderDesc& desc);
    ResourceHash hash_resource_desc(const ShaderDesc& desc);
    ResourceHash hash_resource_desc(const RasterStateDesc& desc);
    ResourceHash hash_resource_desc(const PipelineStateDesc& desc);
  }
}