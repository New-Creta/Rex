#include "rex_directx/directx_util.h"
#include "rex_directx/utility/vertex.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_directx/rendering/scene.h"
#include "rex_directx/rendering/scene_renderer.h"
#include "rex_directx/utility/math_helper.h"

#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/resources/mesh.h"

#include "rex_engine/core_application.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/windowinfo.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/memory/memory_allocation.h"
#include "rex_engine/primitives/mesh_factory.h"
#include "rex_engine/primitives/box.h"

#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"

#include "rex_windows/gui_application.h"
#include "rex_windows/platform_creation_params.h"

#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

DEFINE_LOG_CATEGORY(LogRegina, rex::LogVerbosity::Log);

namespace rex
{
    struct ObjectConstants
    {
        DirectX::XMFLOAT4X4 world = renderer::math_helper::Identity4x4();
    };

    struct PassConstants
    {
        DirectX::XMFLOAT4X4 view = renderer::math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 inv_view = renderer::math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 proj = renderer::math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 inv_proj = renderer::math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 view_proj = renderer::math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 inv_view_proj = renderer::math_helper::Identity4x4();

        DirectX::XMFLOAT3 eye_pos_w = { 0.0f, 0.0f, 0.0f };
        f32 cb_padding_1 = 0.0f;

        DirectX::XMFLOAT2 render_target_size = { 0.0f, 0.0f };
        DirectX::XMFLOAT2 inv_render_target_size = { 0.0f, 0.0f };

        f32 near_z = 0.0f;
        f32 far_z = 0.0f;

        f32 total_time = 0.0f;
        f32 delta_time = 0.0f;
    };

    struct FrameData
    {
        renderer::ResourceSlot frame;
        renderer::ResourceSlot object_constant_buffer;
        renderer::ResourceSlot pass_constant_buffer;
    };

    struct ReginaContext
    {
        rsl::vector<FrameData> frame_resource_data;

        rsl::unique_ptr<renderer::Mesh> mesh_cube = nullptr;

        rsl::unique_ptr<renderer::Scene> scene;
        rsl::unique_ptr<renderer::SceneRenderer> scene_renderer;

        renderer::ResourceSlot clear_state;

        renderer::ResourceSlot shader_program;
        renderer::ResourceSlot input_layout;
        renderer::ResourceSlot pso;

        renderer::ResourceSlot solid_raster_state;
        renderer::ResourceSlot wire_raster_state;

        PassConstants pass_constants;

        bool is_wireframe = false;

        DirectX::XMFLOAT3 eye_pos = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT4X4 view = renderer::math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 proj = renderer::math_helper::Identity4x4();
    };

    ReginaContext g_regina_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    //-------------------------------------------------------------------------
    renderer::ResourceSlot get_active_object_constant_buffer_for_frame(s32 frame)
    {
        auto it = rsl::find_if(rsl::cbegin(g_regina_ctx.frame_resource_data), rsl::cend(g_regina_ctx.frame_resource_data), 
            [frame](const FrameData& data)
        {
                return frame == data.frame.slot_id();
        });

        return it != rsl::cend(g_regina_ctx.frame_resource_data)
            ? it->object_constant_buffer
            : renderer::ResourceSlot::make_invalid();
    }
    //-------------------------------------------------------------------------
    renderer::ResourceSlot get_active_pass_constant_buffer_for_frame(s32 frame)
    {
        auto it = rsl::find_if(rsl::cbegin(g_regina_ctx.frame_resource_data), rsl::cend(g_regina_ctx.frame_resource_data),
            [frame](const FrameData& data)
            {
                return frame == data.frame.slot_id();
            });

        return it != rsl::cend(g_regina_ctx.frame_resource_data)
            ? it->pass_constant_buffer
            : renderer::ResourceSlot::make_invalid();
    }

    //-------------------------------------------------------------------------
    renderer::commands::CompileShader create_compile_shader_parameters(const rsl::small_stack_string& shaderName, renderer::ShaderType shaderType, rsl::string_view filePath)
    {
        memory::Blob file_memory = vfs::open_read(filePath);

        renderer::commands::CompileShader compile_shader_params;

        switch (shaderType)
        {
        case renderer::ShaderType::VERTEX:
            compile_shader_params.shader_entry_point = rsl::tiny_stack_string("VS");
            compile_shader_params.shader_feature_target = rsl::tiny_stack_string("vs_5_1");
            break;
        case renderer::ShaderType::PIXEL:
            compile_shader_params.shader_entry_point = rsl::tiny_stack_string("PS");
            compile_shader_params.shader_feature_target = rsl::tiny_stack_string("ps_5_1");
            break;
        }

        compile_shader_params.shader_code = (char8*)memory_alloc(file_memory.size() * sizeof(char8));
        memcpy(compile_shader_params.shader_code, file_memory.data_as<char8>(), file_memory.size());
        compile_shader_params.shader_code_size = file_memory.size();
        compile_shader_params.shader_name = shaderName;
        compile_shader_params.shader_type = shaderType;

        return compile_shader_params;
    }

    //-------------------------------------------------------------------------
    template<typename T>
    renderer::commands::CreateBuffer create_buffer_parameters(T* data, s32 num)
    {
        renderer::commands::CreateBuffer create_buffer_params;

        create_buffer_params.data = (void*)&data[0];
        create_buffer_params.buffer_size = sizeof(T) * num;

        return create_buffer_params;
    }

    //-------------------------------------------------------------------------
    bool build_clear_state()
    {
        renderer::commands::ClearState create_clear_state_params;

        create_clear_state_params.rgba = { 0.690196097f, 0.768627524f, 0.870588303f, 1.f };
        create_clear_state_params.depth = 1.0f;
        create_clear_state_params.stencil = 0x00;

        StateController<renderer::ClearBits> clear_flags;
        clear_flags.add_state(renderer::ClearBits::CLEAR_COLOR_BUFFER);
        clear_flags.add_state(renderer::ClearBits::CLEAR_DEPTH_BUFFER);
        clear_flags.add_state(renderer::ClearBits::CLEAR_STENCIL_BUFFER);

        create_clear_state_params.flags = clear_flags;

        g_regina_ctx.clear_state = renderer::create_clear_state(create_clear_state_params);

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_shader_and_input_layout()
    {
        // Shader
        renderer::commands::ConstantLayoutDescription constants[2] = 
        {
            {renderer::commands::ConstantType::CBUFFER, "ObjectConstants", 0},
            {renderer::commands::ConstantType::CBUFFER, "PassConstants", 1}
        };

        renderer::commands::CompileShader vs_compile_params = create_compile_shader_parameters(rsl::small_stack_string("standardVS"), renderer::ShaderType::VERTEX, "Shaders\\color.hlsl");
        renderer::commands::CompileShader ps_compile_params = create_compile_shader_parameters(rsl::small_stack_string("opaquePS"), renderer::ShaderType::PIXEL, "Shaders\\color.hlsl");
        renderer::commands::LinkShader link_shader_params;
        link_shader_params.vertex_shader = renderer::compile_shader(vs_compile_params);
        link_shader_params.pixel_shader = renderer::compile_shader(ps_compile_params);
        link_shader_params.constants = constants;
        link_shader_params.num_constants = _countof(constants);
        g_regina_ctx.shader_program = renderer::link_shader(link_shader_params);

        // Input Layout
        renderer::commands::InputLayoutDescription input_layout_descs[2] =
        {
            { "POSITION", 0, renderer::VertexBufferFormat::FLOAT3, 0, 0, renderer::InputLayoutClassification::PER_VERTEX_DATA, 0 },
            { "COLOR", 0, renderer::VertexBufferFormat::FLOAT4, 0, 12, renderer::InputLayoutClassification::PER_VERTEX_DATA, 0 }
        };

        renderer::commands::CreateInputLayout input_layout_params;
        input_layout_params.input_layout = input_layout_descs;
        input_layout_params.num_elements = _countof(input_layout_descs);
        g_regina_ctx.input_layout = renderer::create_input_layout(input_layout_params);

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_cube_geometry()
    {
        auto box = mesh_factory::create_box<u16>(1.5f, 0.5f, 1.5f, 0);

        auto total_vertex_count = box.vertices().size();
        auto total_index_count = box.indices().size();

        rsl::vector<renderer::directx::VertexPosCol> box_vertices((rsl::Capacity)total_vertex_count);
        for (const mesh_factory::Vertex& v : box.vertices())
        {
            renderer::directx::VertexPosCol nv({ DirectX::XMFLOAT3(v.position.x, v.position.y, v.position.z), DirectX::XMFLOAT4(v.position.x, v.position.y, v.position.z, 1.0f)});
            box_vertices.push_back(nv);
        }

        rsl::vector<u16> box_indices((rsl::Capacity)total_index_count);
        box_indices.insert(box_indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));

        const u32 vb_byte_size = total_vertex_count * sizeof(renderer::directx::VertexPosCol);
        const u32 ib_byte_size = total_index_count * sizeof(u16);

        g_regina_ctx.mesh_cube = rsl::make_unique<renderer::Mesh>();
        g_regina_ctx.mesh_cube->name = rsl::medium_stack_string("box_geometry");

        renderer::commands::CreateBuffer v_create_buffer_params = create_buffer_parameters<renderer::directx::VertexPosCol>(box_vertices.data(), box_vertices.size());
        g_regina_ctx.mesh_cube->vertex_buffer = renderer::create_vertex_buffer(v_create_buffer_params);
        renderer::commands::CreateBuffer i_create_buffer_params = create_buffer_parameters<u16>(box_indices.data(), box_indices.size());
        g_regina_ctx.mesh_cube->index_buffer = renderer::create_index_buffer(i_create_buffer_params);

        g_regina_ctx.mesh_cube->vertex_byte_stride = sizeof(renderer::directx::VertexPosCol);
        g_regina_ctx.mesh_cube->vertex_buffer_byte_size = vb_byte_size;
        g_regina_ctx.mesh_cube->index_format = renderer::IndexBufferFormat::R16_UINT;
        g_regina_ctx.mesh_cube->index_buffer_byte_size = ib_byte_size;

        renderer::Submesh submesh;
        submesh.index_count = total_index_count;
        submesh.start_index_location = 0;
        submesh.base_vertex_location = 0;

        g_regina_ctx.mesh_cube->draw_args[rsl::small_stack_string("box")] = submesh;

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_render_items()
    {
        auto cube_r_item = renderer::RenderItem();

        DirectX::XMStoreFloat4x4(&cube_r_item.world, DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f));
        cube_r_item.constant_buffer_index = 0;
        cube_r_item.geometry = g_regina_ctx.mesh_cube.get();
        cube_r_item.topology = renderer::PrimitiveTopology::TRIANGLELIST;
        cube_r_item.index_count = cube_r_item.geometry->draw_args[rsl::small_stack_string("box")].index_count;
        cube_r_item.start_index_location = cube_r_item.geometry->draw_args[rsl::small_stack_string("box")].start_index_location;
        cube_r_item.base_vertex_location = cube_r_item.geometry->draw_args[rsl::small_stack_string("box")].base_vertex_location;

        // Dirty flag indicating the object data has changed and we need to update the constant buffer.
        // Because we have an object cbuffer for each FrameResource, we have to apply the
        // update to each FrameResource. 
        // 
        // Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources 
        // so that each frame resource gets the update.
        cube_r_item.num_frames_dirty = renderer::num_frames_in_flight();

        g_regina_ctx.scene->add_render_item(rsl::move(cube_r_item));

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_frame_resources()
    {
        for (int i = 0; i < renderer::num_frames_in_flight(); ++i)
        {
            renderer::ResourceSlot frame = renderer::create_frame_resource();

            g_regina_ctx.frame_resource_data.push_back({ frame, renderer::ResourceSlot::make_invalid(), renderer::ResourceSlot::make_invalid() });
        }

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_constant_buffers()
    {
        s32 num_render_items = 1;

        // Need a CBV descriptor for each object for each frame resource.
        for (s32 frame = 0; frame < renderer::num_frames_in_flight(); ++frame)
        {
            for (s32 i = 0; i < num_render_items; ++i)
            {
                renderer::commands::CreateConstantBuffer create_const_buffer_params;

                create_const_buffer_params.count = num_render_items;
                create_const_buffer_params.buffer_size = sizeof(ObjectConstants);
                create_const_buffer_params.array_index = (frame * num_render_items) + i;

                renderer::ResourceSlot object_constant_buffer = renderer::create_constant_buffer(create_const_buffer_params);

                g_regina_ctx.frame_resource_data[frame].object_constant_buffer = object_constant_buffer;
            }
        }

        // Last three descriptors are the pass CBVs for each frame resource.
        for (s32 frame = 0; frame < renderer::num_frames_in_flight(); ++frame)
        {
            renderer::commands::CreateConstantBuffer create_const_buffer_params;

            create_const_buffer_params.count = 1;
            create_const_buffer_params.buffer_size = sizeof(PassConstants);
            create_const_buffer_params.array_index = g_regina_ctx.frame_resource_data.size() + frame;

            renderer::ResourceSlot pass_constant_buffer = renderer::create_constant_buffer(create_const_buffer_params);

            g_regina_ctx.frame_resource_data[frame].pass_constant_buffer = pass_constant_buffer;
        }

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_raster_state()
    {
        renderer::commands::RasterState solid_rs;
        solid_rs.fill_mode = renderer::FillMode::SOLID;
        solid_rs.cull_mode = renderer::CullMode::BACK;
        g_regina_ctx.solid_raster_state = renderer::create_raster_state(solid_rs);

        renderer::commands::RasterState wire_rs;
        wire_rs.fill_mode = renderer::FillMode::SOLID;
        wire_rs.cull_mode = renderer::CullMode::BACK;
        g_regina_ctx.wire_raster_state = renderer::create_raster_state(wire_rs);

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_pipeline_state_object()
    {
        renderer::commands::CreatePipelineState create_pso_params;
        create_pso_params.input_layout = g_regina_ctx.input_layout;
        create_pso_params.num_render_targets = 1;
        create_pso_params.shader_program = g_regina_ctx.shader_program;
        create_pso_params.rasterizer_state = g_regina_ctx.solid_raster_state;
        g_regina_ctx.pso = renderer::create_pipeline_state_object(create_pso_params);

        return true;
    }

    //-------------------------------------------------------------------------
    void update_view()
    {
        // Convert Spherical to Cartesian coordinates.
        g_regina_ctx.eye_pos.x = 5.0f * sinf(DirectX::XM_PIDIV4) * cosf(1.5f * DirectX::XM_PI);
        g_regina_ctx.eye_pos.y = 5.0f * sinf(DirectX::XM_PIDIV4) * sinf(1.5f * DirectX::XM_PI);
        g_regina_ctx.eye_pos.z = 5.0f * cosf(DirectX::XM_PIDIV4);

        // Build the view matrix.
        DirectX::XMVECTOR pos = DirectX::XMVectorSet(g_regina_ctx.eye_pos.x, g_regina_ctx.eye_pos.y, g_regina_ctx.eye_pos.z, 1.0f);
        DirectX::XMVECTOR target = DirectX::XMVectorZero();
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
        DirectX::XMStoreFloat4x4(&g_regina_ctx.view, view);
    }
    //-------------------------------------------------------------------------
    void update_object_constant_buffers()
    {
        renderer::ResourceSlot curr_object_cb = get_active_object_constant_buffer_for_frame(renderer::active_frame());

        for (auto& ri : (*g_regina_ctx.scene))
        {
            // Only update the cbuffer data if the constants have changed.  
            // This needs to be tracked per frame resource.
            if (ri.num_frames_dirty > 0)
            {
                // Define the rotation angle (in radians) for each frame
                f32 rotation_angle = 1.0f * globals::frame_info().delta_time().to_seconds(); // You can adjust this value for the desired rotation speed

                // Create a rotation matrix using DirectXMath
                DirectX::XMMATRIX rotation_matrix = DirectX::XMMatrixRotationZ(rotation_angle); // Change the axis and angle as needed

                // Convert the existing world matrix to XMMATRIX
                DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&ri.world);

                // Multiply the current world matrix with the rotation matrix
                DirectX::XMMATRIX new_world = DirectX::XMMatrixMultiply(world, rotation_matrix);

                // Store the result back into the world matrix
                DirectX::XMStoreFloat4x4(&ri.world, new_world);

                ObjectConstants obj_constants;
                DirectX::XMStoreFloat4x4(&obj_constants.world, XMMatrixTranspose(new_world));

                renderer::commands::UpdateConstantBuffer update_constant_buffer_params;
                update_constant_buffer_params.element_index = ri.constant_buffer_index;
                update_constant_buffer_params.data = &obj_constants;
                update_constant_buffer_params.data_size = sizeof(ObjectConstants);

                renderer::update_constant_buffer(update_constant_buffer_params, curr_object_cb);

                // Updating constant buffer of the cube so the frame should remain dirty
                ri.num_frames_dirty = renderer::num_frames_in_flight();
            }
        }
    }
    //-------------------------------------------------------------------------
    void update_pass_constant_buffers()
    {
        renderer::ResourceSlot curr_pass_cb = get_active_pass_constant_buffer_for_frame(renderer::active_frame());

        DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&g_regina_ctx.view);
        DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&g_regina_ctx.proj);
        DirectX::XMMATRIX view_proj = DirectX::XMMatrixMultiply(view, proj);

        auto d_view = DirectX::XMMatrixDeterminant(view);
        DirectX::XMMATRIX inv_view = DirectX::XMMatrixInverse(&d_view, view);
        auto d_proj = DirectX::XMMatrixDeterminant(proj);
        DirectX::XMMATRIX inv_proj = DirectX::XMMatrixInverse(&d_proj, proj);
        auto d_view_proj = DirectX::XMMatrixDeterminant(view_proj);
        DirectX::XMMATRIX inv_view_proj = DirectX::XMMatrixInverse(&d_view_proj, view_proj);

        DirectX::XMStoreFloat4x4(&g_regina_ctx.pass_constants.view, DirectX::XMMatrixTranspose(view));
        DirectX::XMStoreFloat4x4(&g_regina_ctx.pass_constants.inv_view, DirectX::XMMatrixTranspose(inv_view));
        DirectX::XMStoreFloat4x4(&g_regina_ctx.pass_constants.proj, DirectX::XMMatrixTranspose(proj));
        DirectX::XMStoreFloat4x4(&g_regina_ctx.pass_constants.inv_proj, DirectX::XMMatrixTranspose(inv_proj));
        DirectX::XMStoreFloat4x4(&g_regina_ctx.pass_constants.view_proj, DirectX::XMMatrixTranspose(view_proj));
        DirectX::XMStoreFloat4x4(&g_regina_ctx.pass_constants.inv_view_proj, DirectX::XMMatrixTranspose(inv_view_proj));

        f32 window_width = static_cast<f32>(globals::window_info().width);
        f32 window_height = static_cast<f32>(globals::window_info().height);

        g_regina_ctx.pass_constants.eye_pos_w = g_regina_ctx.eye_pos;
        g_regina_ctx.pass_constants.render_target_size = DirectX::XMFLOAT2(window_width, window_height);
        g_regina_ctx.pass_constants.inv_render_target_size = DirectX::XMFLOAT2(1.0f / window_width, 1.0f / window_height);
        g_regina_ctx.pass_constants.near_z = globals::default_depth_info().near_plane;
        g_regina_ctx.pass_constants.far_z = globals::default_depth_info().far_plane;
        g_regina_ctx.pass_constants.delta_time = globals::frame_info().delta_time().to_seconds();

        renderer::commands::UpdateConstantBuffer update_constant_buffer_params;
        update_constant_buffer_params.element_index = 0;
        update_constant_buffer_params.data = &g_regina_ctx.pass_constants;
        update_constant_buffer_params.data_size = sizeof(PassConstants);

        renderer::update_constant_buffer(update_constant_buffer_params, curr_pass_cb);
    }

    //-------------------------------------------------------------------------
    bool initialize()
    {
        REX_LOG(LogRegina, "Initializing Regina");

        g_regina_ctx.scene = rsl::make_unique<renderer::Scene>();
        g_regina_ctx.scene_renderer = rsl::make_unique<renderer::SceneRenderer>(g_regina_ctx.scene.get());

        if (!build_clear_state()) return false;
        if (!build_shader_and_input_layout()) return false;
        if (!build_cube_geometry()) return false;
        if (!build_render_items()) return false;
        if (!build_frame_resources()) return false;
        if (!build_constant_buffers()) return false;
        if (!build_raster_state()) return false;
        if (!build_pipeline_state_object()) return false;

        // The window resized, so update the aspect ratio and recompute the projection matrix.
        f32 aspect_ratio = static_cast<f32>(globals::window_info().width) / static_cast<f32>(globals::window_info().height);
        DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, aspect_ratio, globals::default_depth_info().near_plane, globals::default_depth_info().far_plane);
        DirectX::XMStoreFloat4x4(&g_regina_ctx.proj, P);

        return true;
    }
    //-------------------------------------------------------------------------
    void update()
    {
        update_view();

        // Has the GPU finished processing the commands of the current frame resource?
        // If not, wait until the GPU has completed commands up to this fence point.
        // 
        // This call is not queued but directly executed!
        renderer::wait_for_active_frame();

        update_object_constant_buffers();
        update_pass_constant_buffers();
    }
    //-------------------------------------------------------------------------
    void draw()
    {
        Viewport vp = { 0.0f, 0.0f, (f32)globals::window_info().width, (f32)globals::window_info().height, 0.0f, 1.0f};
        ScissorRect sr = { vp.top_left_x, vp.top_left_y, vp.width, vp.height };

        renderer::set_pipeline_state_object(g_regina_ctx.pso);
        renderer::new_frame();

        renderer::set_render_targets(globals::default_targets_info().back_buffer_color , globals::default_targets_info().depth_buffer);
        renderer::set_viewport(vp);
        renderer::set_scissor_rect(sr);
        renderer::set_raster_state(g_regina_ctx.solid_raster_state);

        renderer::begin_draw();

        renderer::clear(g_regina_ctx.clear_state);

        renderer::set_shader(g_regina_ctx.shader_program);

        renderer::ResourceSlot curr_pass_cb = get_active_pass_constant_buffer_for_frame(renderer::active_frame());

        renderer::set_constant_buffer(curr_pass_cb, 1);

        renderer::set_vertex_buffer(g_regina_ctx.mesh_cube->vertex_buffer, 0, g_regina_ctx.mesh_cube->vertex_byte_stride, 0);
        renderer::set_index_buffer(g_regina_ctx.mesh_cube->index_buffer, renderer::IndexBufferFormat::R16_UINT, 0);
        renderer::set_primitive_topology(renderer::PrimitiveTopology::TRIANGLELIST);

        renderer::ResourceSlot curr_object_cb = get_active_object_constant_buffer_for_frame(renderer::active_frame());
        renderer::set_constant_buffer(curr_object_cb, 0);

        renderer::renderer_draw_indexed_instanced(1, 0, g_regina_ctx.mesh_cube->draw_args[rsl::small_stack_string("box")].index_count, 0, 0);

        renderer::end_draw();

        renderer::present();
        renderer::end_frame();

        renderer::flush();
    }
    //-------------------------------------------------------------------------
    void shutdown()
    {
        REX_LOG(LogRegina, "shutting down Regina");

        renderer::release_resource(g_regina_ctx.clear_state);

        renderer::release_resource(g_regina_ctx.shader_program);
        renderer::release_resource(g_regina_ctx.input_layout);
        renderer::release_resource(g_regina_ctx.pso);

        for(auto& data : g_regina_ctx.frame_resource_data)
        {
            renderer::release_resource(data.frame);
            renderer::release_resource(data.object_constant_buffer);
            renderer::release_resource(data.pass_constant_buffer);
        }

        renderer::release_resource(g_regina_ctx.solid_raster_state);
        renderer::release_resource(g_regina_ctx.wire_raster_state);

        g_regina_ctx.mesh_cube.reset();
    }

    //-------------------------------------------------------------------------
    ApplicationCreationParams create_regina_app_creation_params(PlatformCreationParams&& platformParams)
    {
        ApplicationCreationParams app_params(rsl::move(platformParams));

        app_params.gui_params.window_width = 1280;
        app_params.gui_params.window_height = 720;
        app_params.gui_params.window_title = "Regina";

        app_params.engine_params.max_memory = 256_kb;
        app_params.engine_params.app_init_func = initialize;
        app_params.engine_params.app_update_func = update;
        app_params.engine_params.app_draw_func = draw;
        app_params.engine_params.app_shutdown_func = shutdown;

        app_params.create_window = true;

        return app_params;
    }

#ifndef REX_ENABLE_AUTO_TESTS
    //-------------------------------------------------------------------------
    ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
    {
        return create_regina_app_creation_params(rsl::move(platformParams));
    }

#endif
} // namespace rex
