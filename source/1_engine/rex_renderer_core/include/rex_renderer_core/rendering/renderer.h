#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_std/bonus/utility/yes_no.h"

namespace rex
{
  struct Viewport;
  struct ScissorRect;

  namespace rhi
  {
    class ResourceSlot;
  } // namespace rhi

  namespace renderer
  {
    DEFINE_YES_NO_ENUM(FlushCommands);

    struct OutputWindowUserData;

    class RenderItem;
    class Mesh;

    enum class ShaderPlatform;
    enum class PrimitiveTopology;
    enum class IndexBufferFormat;

    bool initialize(const OutputWindowUserData& userData);
    void render();
    void shutdown();

    RenderItem* add_mesh(const Mesh* mesh, const struct Submesh& subMesh, rex::memory::BlobView cbView);

    ShaderPlatform shader_platform();

    bool is_y_up();
    bool is_depth_0_to_1();

    s32 max_frames_in_flight();

    bool new_frame();
    bool end_frame();

    bool begin_draw();
    bool end_draw();

    bool present();
    bool flush();
  } // namespace renderer
} // namespace rex