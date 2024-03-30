#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility/yes_no.h"

#include "rex_engine/memory/blob_view.h"

namespace rex
{
  struct Viewport;
  struct ScissorRect;
  struct DefaultTargetsInfo;
  struct DefaultDepthInfo;

  namespace rhi
  {
    class ResourceSlot;
  }

  namespace globals
  {
    const DefaultTargetsInfo& default_targets_info();
    const DefaultDepthInfo& default_depth_info();
  } // namespace globals

  namespace renderer
  {
    DEFINE_YES_NO_ENUM(FlushCommands);

    struct OutputWindowUserData;

    class Mesh;

    enum class ShaderPlatform;
    enum class PrimitiveTopology;
    enum class IndexBufferFormat;

    bool initialize(const OutputWindowUserData& userData, s32 maxCommands, s32 maxFrameResources);
    void render();
    void shutdown();

    struct RenderItem* add_mesh(const Mesh* mesh, const struct Submesh& subMesh, rex::memory::BlobView cbView);

    ShaderPlatform shader_platform();

    bool is_y_up();
    bool is_depth_0_to_1();

    s32 max_frames_in_flight();

    bool prepare_user_initialization();
    bool finish_user_initialization();

    void set_viewport(const Viewport& viewport);
    void set_scissor_rect(const ScissorRect& rect);

    void set_shader(const rhi::ResourceSlot& slot);
    void set_pso(const rhi::ResourceSlot& slot);

    bool new_frame();
    bool end_frame(FlushCommands flush = FlushCommands::yes);

    bool begin_draw();
    bool end_draw();

    bool present();
    bool flush();
  } // namespace renderer
} // namespace rex