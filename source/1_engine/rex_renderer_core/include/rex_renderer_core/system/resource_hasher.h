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
    ResourceID hash_resource_desc(const VertexBufferDesc& desc);
    ResourceID hash_resource_desc(const IndexBufferDesc& desc);
    ResourceID hash_resource_desc(const ConstantBufferDesc& desc);
    ResourceID hash_resource_desc(const ClearStateDesc& desc);
    ResourceID hash_resource_desc(const CompileShaderDesc& desc);
    ResourceID hash_resource_desc(const InputLayoutDesc& desc);
    ResourceID hash_resource_desc(const LinkShaderDesc& desc);
    ResourceID hash_resource_desc(const ShaderDesc& desc);
    ResourceID hash_resource_desc(const RasterStateDesc& desc);
    //ResourceID hash_resource_desc(const PipelineStateDesc& desc);
  }
}