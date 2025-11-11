#include "rex_directx/system/dx_gal.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/system/dx_shader_compiler.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/system/dx_fence.h"
#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_unordered_access_buffer.h"
#include "rex_engine/gfx/system/graphics_context.h"

#include "rex_directx/resources/dx_input_layout.h"
#include "rex_directx/resources/dx_vertex_shader.h"
#include "rex_directx/resources/dx_pixel_shader.h"
#include "rex_directx/resources/dx_pipeline_state.h"
#include "rex_directx/resources/dx_render_target.h"
#include "rex_directx/resources/dx_depth_stencil_buffer.h"
#include "rex_directx/shader_reflection/dx_shader_reflection.h"

#include "rex_engine/gfx/graphics.h"

#include "rex_directx/resources/dx_root_signature.h"

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"

#include "rex_engine/gfx/core/renderer_output_window_user_data.h"
#include "rex_engine/gfx/core/viewport.h"
#include "rex_engine/gfx/core/scissor_rect.h"

#include "rex_std/bonus/utility.h"

#include <D3Dcompiler.h>
#include "rex_directx/dxgi/includes.h"

#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_directx/shader_reflection/dx_shader_reflection.h"
#include "rex_directx/system/dx_shader_root_parameters.h"
#include "rex_directx/resources/dx_sampler_2d.h"

#include "rex_engine/gfx/materials/material_library.h"
#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/system/resource_cache.h"
#include "rex_engine/gfx/system/resource_manager.h"



#include "rex_directx/system/dx_render_engine.h"
#include "rex_directx/system/dx_compute_engine.h"


namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogDxRhi);

		// Startup initialization
		namespace internal
		{
			DEFINE_YES_NO_ENUM(EnableDebugFactory);
			rsl::unique_ptr<dxgi::Factory> create_dxgi_factory(EnableDebugFactory enableDebugFactory)
			{
				s32 dxgi_factory_flags = 0;
				if (enableDebugFactory)
				{
					dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
				}

				return dxgi::Factory::create(dxgi_factory_flags);
			}
			rsl::unique_ptr<DxDevice> create_d3d_device(dxgi::AdapterManager* adapterManager)
		{
			// Select highest scoring gpu
			const dxgi::Adapter* selected_gpu = adapterManager->selected();
			rsl::unique_ptr<DxDevice> device = selected_gpu->create_device();

			if (!device)
			{
				REX_ERROR(LogDxRhi, "Failed to create DX12 Device");
				return nullptr;
			}

			return device;
		}
		}

		DirectXInterface::DirectXInterface(const OutputWindowUserData& userData)
			: GALBase(userData)
			, m_rtv_clear_color(rsl::colors::White)
		{
		}

		DirectXInterface::~DirectXInterface()
		{
			resource_cache::shutdown();
			shader_reflection::shutdown();
			material_lib::shutdown();
		}

		// Return information about the graphics hardware and software
		const Info& DirectXInterface::info() const
		{
			return m_device->info();
		}

		// Return the shader platform used for this API.
		ShaderPlatform DirectXInterface::shader_platform() const
		{
			return ShaderPlatform::Hlsl;
		}

		rsl::unique_ptr<DxFence>          DirectXInterface::create_fence()
		{
			wrl::ComPtr<ID3D12Fence> fence;
			if (DX_FAILED(m_device->dx_object()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
			{
				REX_ERROR(LogDxRhi, "Failed to create DX fence, to synchronize CPU/GPU");
				return nullptr;
			}

			return rsl::make_unique<DxFence>(fence);
		}
		rsl::unique_ptr<CommandQueue>     DirectXInterface::create_command_queue(GraphicsEngineType type)
		{
			// Command Queue
			wrl::ComPtr<ID3D12CommandQueue> d3d_command_queue;
			D3D12_COMMAND_QUEUE_DESC queue_desc = {};
			queue_desc.Type = d3d::to_dx12(type);
			queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			if (DX_FAILED(m_device->dx_object()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(d3d_command_queue.GetAddressOf()))))
			{
				REX_ERROR(LogDxRhi, "Failed to create command queue");
				return nullptr;
			}

			switch (type)
			{
			case GraphicsEngineType::Render: d3d::set_debug_name_for(d3d_command_queue.Get(), "Render Command Queue"); break;
			case GraphicsEngineType::Copy: d3d::set_debug_name_for(d3d_command_queue.Get(), "Copy Command Queue"); break;
			case GraphicsEngineType::Compute: d3d::set_debug_name_for(d3d_command_queue.Get(), "Compute Command Queue"); break;
			}

			rsl::unique_ptr<DxFence> fence = create_fence();
			return rsl::make_unique<DxCommandQueue>(type, d3d_command_queue, rsl::move(fence));
		}
		rsl::unique_ptr<Swapchain>        DirectXInterface::create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle)
		{
			DXGI_SWAP_CHAIN_DESC1 sd{};
			sd.Width = 0;
			sd.Height = 0;
			sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = bufferCount;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			sd.Stereo = false;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

			// Note: swap chain uses queue to perform flush.
			wrl::ComPtr<IDXGIFactory4> dxgi_factory = m_factory->as<IDXGIFactory4>();
			wrl::ComPtr<IDXGISwapChain1> d3d_swapchain;
			CommandQueue* cmd_queue = (CommandQueue*)apiDevice;
			DxCommandQueue* dx_cmd_queue = d3d::to_dx12(cmd_queue);
			if (DX_FAILED(dxgi_factory->CreateSwapChainForHwnd(dx_cmd_queue->dx_object(), (HWND)primaryDisplayHandle, &sd, nullptr, nullptr, d3d_swapchain.GetAddressOf())))
			{
				REX_ERROR(LogDxRhi, "Failed to create swap chain");
				return nullptr;
			}

			wrl::ComPtr<IDXGISwapChain3> d3d_swapchain_3;
			DX_CALL(d3d_swapchain.As(&d3d_swapchain_3));

			d3d_swapchain_3->GetDesc1(&sd);
			s32 width = sd.Width;
			s32 height = sd.Height;

			return rsl::make_unique<DxSwapchain>(d3d_swapchain_3, width, height, sd.Format, sd.BufferCount);
		}
		rsl::unique_ptr<CommandAllocator> DirectXInterface::create_command_allocator(GraphicsEngineType type)
		{
			wrl::ComPtr<ID3D12CommandAllocator> allocator;
			if (DX_FAILED(m_device->dx_object()->CreateCommandAllocator(d3d::to_dx12(type), IID_PPV_ARGS(allocator.GetAddressOf()))))
			{
				REX_ERROR(LogDxRhi, "Failed to create command allocator");
				return nullptr;
			}

			switch (type)
			{
			case GraphicsEngineType::Render: d3d::set_debug_name_for(allocator.Get(), "Render Command Allocator"); break;
			case GraphicsEngineType::Copy: d3d::set_debug_name_for(allocator.Get(), "Copy Command Allocator"); break;
			case GraphicsEngineType::Compute: d3d::set_debug_name_for(allocator.Get(), "Compute Command Allocator"); break;
			}

			return rsl::make_unique<DxCommandAllocator>(allocator);
		}

		// Resource creation
		// -------------------------------------------
		rsl::unique_ptr<VertexBuffer>         DirectXInterface::create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data)
		{
			s32 total_size = numVertices * vertexSize;
			wrl::ComPtr<ID3D12Resource> d3d_buffer = allocate_buffer(total_size);
			auto vb = rsl::make_unique<DxVertexBuffer>(d3d_buffer, numVertices, vertexSize);
			d3d::set_debug_name_for(d3d_buffer.Get(), "Vertex Buffer");

			if (data)
			{
				auto copy_context = gfx::gal::instance()->new_render_ctx();
				copy_context->update_buffer(vb.get(), data, total_size, 0);
				auto sync_info = copy_context->execute_on_gpu();

				auto render_context = gfx::gal::instance()->new_render_ctx();
				render_context->stall(*sync_info.get());
				render_context->transition_buffer(vb.get(), ResourceState::VertexAndConstantBuffer);
			}

			return vb;
		}
		rsl::unique_ptr<IndexBuffer>          DirectXInterface::create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data)
		{
			s32 index_size = index_format_size(format);
			s32 total_size = numIndices * index_size;
			wrl::ComPtr<ID3D12Resource> buffer = allocate_buffer(total_size);
			d3d::set_debug_name_for(buffer.Get(), "Index Buffer");

			auto ib = rsl::make_unique<DxIndexBuffer>(buffer, numIndices, format);
			if (data)
			{
				auto copy_context = gfx::gal::instance()->new_render_ctx();
				copy_context->update_buffer(ib.get(), data, total_size, 0);
				auto sync_info = copy_context->execute_on_gpu();

				auto render_context = gfx::gal::instance()->new_render_ctx();
				render_context->stall(*sync_info.get());
				render_context->transition_buffer(ib.get(), ResourceState::IndexBuffer);
			}

			return ib;
		}
		rsl::unique_ptr<RootSignature>        DirectXInterface::create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters)
		{
			DxShaderPipelineParameters dx_pipeline_parameters = d3d::to_dx12(parameters);

			// A root signature is an array of root parameters.
			REX_WARN(LogDxRhi, "Use versioned root signature here");
			REX_WARN(LogDxRhi, "Investigate if we can use static samplers here as well..");
			CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(
				dx_pipeline_parameters.root_parameters.size(),
				dx_pipeline_parameters.root_parameters.data(),
				0,          // As we're creating the root signature from reflection, we cannot infer the static samplers at the moment
				nullptr,    // As we're creating the root signature from reflection, we cannot infer the static samplers at the moment
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
			wrl::ComPtr<ID3DBlob> serialized_root_sig = nullptr;
			wrl::ComPtr<ID3DBlob> error_blob = nullptr;

			HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());
			if (error_blob != nullptr)
			{
				REX_ERROR(LogDxRhi, "{}", (char*)error_blob->GetBufferPointer());
				return nullptr;
			}

			if (DX_FAILED(hr))
			{
				REX_ERROR(LogDxRhi, "Failed to serialize root signature");
				return nullptr;
			}

			wrl::ComPtr<ID3D12RootSignature> root_signature;
			if (DX_FAILED(m_device->dx_object()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&root_signature))))
			{
				HR_CALL(m_device->dx_object()->GetDeviceRemovedReason());
				REX_ERROR(LogDxRhi, "Failed to create root signature");
				return nullptr;
			}

			return rsl::make_unique<DxRootSignature>(root_signature);
		}

		rsl::unique_ptr<RenderTarget>         DirectXInterface::create_render_target(s32 width, s32 height, TextureFormat format)
		{
			ClearStateDesc clear_state{};
			clear_state.rgba = m_rtv_clear_color;
			clear_state.flags.add_state(ClearBits::ClearColorBuffer);

			wrl::ComPtr<ID3D12Resource> d3d_texture = allocate_render_target(width, height, format, clear_state);
			return create_render_target(d3d_texture, d3d::to_dx12(format));
		}
		rsl::unique_ptr<PipelineState>        DirectXInterface::create_pso(const PipelineStateDesc& desc)
		{
			REX_ASSERT_X(desc.shader_pipeline.vs, "No vertex shader specified for the pso");
			REX_ASSERT_X(desc.shader_pipeline.ps, "No pixel shader specified for the pso");

			InputLayout* input_layout = resource_cache::instance()->load_input_layout(desc.input_layout);
			RootSignature* root_signature = resource_cache::instance()->load_root_signature(desc.shader_pipeline);

			// Make sure our critical required parameters are specified
			REX_ASSERT_X(input_layout, "No input layout for the pso");
			REX_ASSERT_X(root_signature, "No root signature for the pso");

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
			pso_desc.InputLayout = d3d::to_dx12(input_layout)->dx_object();
			pso_desc.pRootSignature = d3d::to_dx12(root_signature)->dx_object();
			pso_desc.VS = d3d::to_dx12(desc.shader_pipeline.vs)->dx_bytecode();
			pso_desc.PS = d3d::to_dx12(desc.shader_pipeline.ps)->dx_bytecode();
			pso_desc.RasterizerState = d3d::to_dx12(desc.output_merger.raster_state);
			pso_desc.BlendState = d3d::to_dx12(desc.output_merger.blend_state);
			pso_desc.DepthStencilState = d3d::to_dx12(desc.output_merger.depth_stencil_state);
			pso_desc.SampleMask = UINT_MAX;
			pso_desc.PrimitiveTopologyType = d3d::to_dx12(desc.primitive_topology);
			pso_desc.NumRenderTargets = 1;
			pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			pso_desc.DSVFormat = d3d::to_dx12(desc.dsv_format);
			pso_desc.SampleDesc.Count = 1;
			pso_desc.SampleDesc.Quality = 0;

			wrl::ComPtr<ID3D12PipelineState> pso;
			DX_CALL(m_device->dx_object()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso)));
			if (!pso)
			{
				return nullptr;
			}

			return rsl::make_unique<DxPipelineState>(pso, root_signature);
		}
		rsl::unique_ptr<Texture2D>            DirectXInterface::create_texture2d(s32 width, s32 height, TextureFormat format, const void* data)
		{
			TextureFormat format_for_gpu = format;
			if (format == TextureFormat::Unorm3)
			{
				format_for_gpu = TextureFormat::Unorm4;
			}
			wrl::ComPtr<ID3D12Resource> d3d_texture = allocate_texture2d(width, height, format_for_gpu);
			DxResourceView desc_handle = create_texture2d_srv(d3d_texture.Get());

			auto texture = rsl::make_unique<DxTexture2D>(d3d_texture, desc_handle, width, height, format_for_gpu);

			// Upload data to gpu and transition to pixel shader resource
			if (data)
			{
				auto copy_context = gfx::gal::instance()->new_render_ctx();
				copy_context->update_texture2d(texture.get(), data);
				auto sync_info = copy_context->execute_on_gpu();

				auto render_context = gfx::gal::instance()->new_render_ctx();
				render_context->stall(*sync_info.get());
				render_context->transition_buffer(texture.get(), ResourceState::PixelShaderResource);
			}

			return texture;
		}
		rsl::unique_ptr<DepthStencilBuffer>   DirectXInterface::create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
		{
			REX_ASSERT_X(clearStateDesc.flags.has_state(ClearBits::ClearDepthBuffer) || clearStateDesc.flags.has_state(ClearBits::ClearStencilBuffer), "Using a clear state for a depth stencil buffer, but depth or stencil bits are not enabled");

			wrl::ComPtr<ID3D12Resource> d3d_texture = allocate_depth_stencil(width, height, format, clearStateDesc);
			DxResourceView desc_handle = create_dsv(d3d_texture.Get());

			auto dx_buffer = rsl::make_unique<DxDepthStencilBuffer>(d3d_texture, desc_handle, width, height, format, clearStateDesc);
			dx_buffer->debug_set_name("Depth Buffer");

			return dx_buffer;
		}
		rsl::unique_ptr<ConstantBuffer>       DirectXInterface::create_constant_buffer(rsl::memory_size size, rsl::string_view debugName)
		{
			// 1) Create the resource on the gpu that'll hold the data of the vertex buffer
			rsl::memory_size aligned_size = align(size.size_in_bytes(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			wrl::ComPtr<ID3D12Resource> d3d_constant_buffer = allocate_buffer(aligned_size);
			DxResourceView desc_handle = create_cbv(d3d_constant_buffer.Get(), aligned_size);

			d3d::set_debug_name_for(d3d_constant_buffer.Get(), debugName);

			return rsl::make_unique<DxConstantBuffer>(d3d_constant_buffer, desc_handle, size);
		}
		rsl::unique_ptr<InputLayout>          DirectXInterface::create_input_layout(const InputLayoutDesc& desc)
		{
			rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Size(desc.size()));
			REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

			for (s32 i = 0; i < desc.size(); ++i)
			{
				input_element_descriptions[i].SemanticName = shader_semantic_name(desc[i].semantic).data();
				input_element_descriptions[i].SemanticIndex = desc[i].semantic_index;
				input_element_descriptions[i].Format = d3d::to_dx12(desc[i].format);
				input_element_descriptions[i].InputSlot = desc[i].input_slot;
				input_element_descriptions[i].AlignedByteOffset = desc[i].aligned_byte_offset;
				input_element_descriptions[i].InputSlotClass = d3d::to_dx12(desc[i].input_slot_class);
				input_element_descriptions[i].InstanceDataStepRate = desc[i].instance_data_step_rate;
			}

			return rsl::make_unique<DxInputLayout>(input_element_descriptions, rsl::move(desc));
		}

		rsl::unique_ptr<Shader>               DirectXInterface::create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
		{
			CompileShaderDesc compile_vs_desc{};
			compile_vs_desc.shader_source_code = sourceCode;
			compile_vs_desc.shader_entry_point = "main";
			compile_vs_desc.shader_feature_target = "vs_5_1";
			compile_vs_desc.shader_name = shaderName;
			compile_vs_desc.shader_type = ShaderType::Vertex;
			wrl::ComPtr<ID3DBlob> compiled_vs_blob = shader_compiler::instance()->compile_shader(compile_vs_desc);

			if (!compiled_vs_blob)
			{
				return nullptr;
			}

			return rsl::make_unique<DxVertexShader>(compiled_vs_blob);
		}
		rsl::unique_ptr<Shader>               DirectXInterface::create_vertex_shader(const memory::Blob& byteBlob)
		{
			wrl::ComPtr<ID3DBlob> d3d_blob = d3d::create_blob(byteBlob);
			return rsl::make_unique<DxVertexShader>(d3d_blob);
		}
		rsl::unique_ptr<Shader>               DirectXInterface::create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
		{
			CompileShaderDesc compile_ps_desc{};
			compile_ps_desc.shader_source_code = sourceCode;
			compile_ps_desc.shader_entry_point = "main";
			compile_ps_desc.shader_feature_target = "ps_5_1";
			compile_ps_desc.shader_name = shaderName;
			compile_ps_desc.shader_type = ShaderType::Pixel;
			wrl::ComPtr<ID3DBlob> compiled_ps_blob = shader_compiler::instance()->compile_shader(compile_ps_desc);

			if (!compiled_ps_blob)
			{
				return nullptr;
			}

			return rsl::make_unique<DxPixelShader>(compiled_ps_blob);
		}
		rsl::unique_ptr<Shader>               DirectXInterface::create_pixel_shader(const memory::Blob& byteBlob)
		{
			wrl::ComPtr<ID3DBlob> d3d_blob = d3d::create_blob(byteBlob);
			return rsl::make_unique<DxPixelShader>(d3d_blob);
		}
		rsl::unique_ptr<UploadBuffer>         DirectXInterface::create_upload_buffer(rsl::memory_size size)
		{
			// an intermediate upload heap.
			CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
			auto buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(size);

			wrl::ComPtr<ID3D12Resource> d3d_upload_buffer;
			if (DX_FAILED(m_device->dx_object()->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(d3d_upload_buffer.GetAddressOf()))))
			{
				REX_ERROR(LogDxRhi, "Failed to create committed resource for intermediate upload heap.");
				return {};
			}

			d3d::set_debug_name_for(d3d_upload_buffer.Get(), "Upload Buffer");
			return rsl::make_unique<DxUploadBuffer>(d3d_upload_buffer);
		}

		rsl::unique_ptr<Material> DirectXInterface::create_material(const MaterialDesc& matDesc)
		{
			return rsl::make_unique<Material>(matDesc);
		}
		rsl::unique_ptr<Sampler2D> DirectXInterface::create_sampler2d(const SamplerDesc& desc)
		{
			return allocate_sampler2d(desc);
		}
		rsl::unique_ptr<UnorderedAccessBuffer> DirectXInterface::create_unordered_access_buffer(rsl::memory_size size, const void* data)
		{
			wrl::ComPtr<ID3D12Resource> d3d_buffer = allocate_unordered_access_buffer(size);
			d3d::set_debug_name_for(d3d_buffer.Get(), "Unordered Access Buffer");
			DxResourceView desc_handle = create_uav(d3d_buffer.Get(), size);

			auto uab = rsl::make_unique<DxUnorderedAccessBuffer>(d3d_buffer, desc_handle, size);

			if (data)
			{
				auto copy_context = gfx::gal::instance()->new_render_ctx();
				copy_context->update_buffer(uab.get(), data, size, 0);
				auto sync_info = copy_context->execute_on_gpu();

				auto render_context = gfx::gal::instance()->new_render_ctx();
				render_context->stall(*sync_info.get());
			}

			return uab;
		}
		
		// View creation
		// -------------------------------------------
		ResourceView* DirectXInterface::create_srv(const RenderTarget* rt)
		{
			ResourceView* cached_view = resource_manager::instance()->shader_resource_view(rt);
			if (cached_view)
			{
				return cached_view;
			}

			DxResourceView view = create_texture2d_srv(d3d::to_dx12(rt)->dx_object());
			rsl::unique_ptr<ResourceView> view_ptr = rsl::make_unique<DxResourceView>(view);
			return resource_manager::instance()->store_shader_resource_view(rt, rsl::move(view_ptr));
		}


		// API Specific functions
		// -------------------------------------------
		rsl::unique_ptr<RenderTarget> DirectXInterface::create_render_target(wrl::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format)
		{
			DxResourceView rtv = create_rtv(resource.Get(), format);
			return rsl::make_unique<DxRenderTarget>(resource, rtv, m_rtv_clear_color);
		}
		rsl::unique_ptr<RenderTarget> DirectXInterface::retarget_render_target(wrl::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format, DxResourceView view)
		{
			d3d::to_dx12(cpu_view_heap(ResourceViewType::RenderTarget))->retarget_rtv(resource.Get(), format, view);
			return rsl::make_unique<DxRenderTarget>(resource, view, m_rtv_clear_color);
		}

		wrl::ComPtr<ID3D12GraphicsCommandList> DirectXInterface::create_commandlist(CommandAllocator* alloc, GraphicsEngineType type)
		{
			DxCommandAllocator* dx_alloc = d3d::to_dx12(alloc);

			wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
			if (DX_FAILED(m_device->dx_object()->CreateCommandList(0, d3d::to_dx12(type), dx_alloc->dx_object(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
			{
				REX_ERROR(LogDxRhi, "Failed to create command list");
				return nullptr;
			}

			switch (type)
			{
			case GraphicsEngineType::Render: d3d::set_debug_name_for(cmd_list.Get(), "Render Command List"); break;
			case GraphicsEngineType::Copy: d3d::set_debug_name_for(cmd_list.Get(), "Copy Command List"); break;
			case GraphicsEngineType::Compute: d3d::set_debug_name_for(cmd_list.Get(), "Compute Command List"); break;
			};

			return cmd_list;
		}
		rsl::unique_ptr<ViewHeap> DirectXInterface::create_view_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, IsShaderVisible isShaderVisible)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};

			s32 num_descriptors = 1024;

			desc.Type = type;
			desc.NumDescriptors = num_descriptors;
			desc.Flags = isShaderVisible
				? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
				: D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )

			wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
			rsl::string_view type_str = rsl::enum_refl::enum_name(type);
			if (DX_FAILED(m_device->dx_object()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap))))
			{
				REX_ERROR(LogDxRhi, "Failed to create descriptor heap for type: {}", type_str);
				return nullptr;
			}

			d3d::set_debug_name_for(desc_heap.Get(), rsl::format("Descriptor Heap - {} - {}", type_str, isShaderVisible ? "Shader Visible" : "Not Shader Visible"));
			s32 desc_size = m_device->dx_object()->GetDescriptorHandleIncrementSize(type);
			s32 total_size = desc_size * num_descriptors;
			REX_UNUSED_PARAM(total_size);

			REX_INFO(LogDxRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, num_descriptors, desc_size, total_size);

			return rsl::make_unique<DxViewHeap>(desc_heap, m_device->dx_object(), isShaderVisible);
		}
		rsl::unique_ptr<DxResourceHeap> DirectXInterface::create_resource_heap()
		{
			CD3DX12_HEAP_DESC desc(100_mib, D3D12_HEAP_TYPE_DEFAULT);

			wrl::ComPtr<ID3D12Heap> d3d_heap;
			if (DX_FAILED(m_device->dx_object()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
			{
				REX_ERROR(LogDxRhi, "Failed to create global resource heap");
				return nullptr;
			}

			return rsl::make_unique<DxResourceHeap>(d3d_heap, m_device->dx_object());
		}
		void DirectXInterface::report_live_objects()
		{
			debug_interface()->report_live_objects();
		}

		void DirectXInterface::api_init()
		{
			// 1) we need to init the dxgi factory.
			// This is the system we use to create most other systems.
			bool enable_debug_factory = debug_interface() != nullptr;
			m_factory = internal::create_dxgi_factory(enable_debug_factory);

			// 2) Create the adapter manager
			// It'll go over all found GPUs in the system and score them
			m_adapter_manager = rsl::make_unique<dxgi::AdapterManager>(m_factory.get(), [](const GpuDescription& gpu) { return calc_gpu_score(gpu); });

			// 3) we need to init the device.
			// A DirectX Device is used for creation of other resources
			// You can think of it as an abstraction of the GPU,
			// but just an abstraction over resource creation,
			// not an abstraction of the gpu itself, that's what an IDXGIAdapter is for.
			m_device = internal::create_d3d_device(m_adapter_manager.get());
			if (!m_device)
			{
				REX_ERROR(LogDxRhi, "Failed to create D3D Device");
				return;
			}

			material_lib::init(globals::make_unique<MaterialLibrary>());
			shader_reflection::init(globals::make_unique<DxShaderReflection>());
			resource_cache::init(globals::make_unique<ResourceCache>());
		}

		// Allocate a 1D buffer on the gpu, returning a DirectX resource
		wrl::ComPtr<ID3D12Resource> DirectXInterface::allocate_buffer(rsl::memory_size size)
		{
			return m_heap->create_buffer(size);
		}
		// Allocate a 1D buffer on the gpu that allows for unordered access, returning a DirectX resource
		wrl::ComPtr<ID3D12Resource> DirectXInterface::allocate_unordered_access_buffer(rsl::memory_size size)
		{
			return m_heap->create_unordered_access_buffer(size);
		}
		// Allocate a 2D buffer on the gpu, returning a DirectX resource
		wrl::ComPtr<ID3D12Resource> DirectXInterface::allocate_texture2d(s32 width, s32 height, TextureFormat format)
		{
			DXGI_FORMAT d3d_format = d3d::to_dx12(format);
			return m_heap->create_texture2d(d3d_format, width, height);
		}
		wrl::ComPtr<ID3D12Resource> DirectXInterface::allocate_render_target(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearColor)
		{
			DXGI_FORMAT d3d_format = d3d::to_dx12(format);
			return m_heap->create_render_target(d3d_format, width, height, clearColor);
		}

		wrl::ComPtr<ID3D12Resource> DirectXInterface::allocate_depth_stencil(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
		{
			DXGI_FORMAT d3d_format = d3d::to_dx12(format);
			return m_heap->create_depth_stencil_buffer(d3d_format, width, height, clearStateDesc);
		}

		// Create a render target view for a given resource
		DxResourceView DirectXInterface::create_rtv(const wrl::ComPtr<ID3D12Resource>& texture, DXGI_FORMAT format)
		{
			return d3d::to_dx12(cpu_view_heap(ResourceViewType::RenderTarget))->create_rtv(texture.Get(), format);
		}
		// Create a shader resource view pointing to a 2D texture
		DxResourceView DirectXInterface::create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture)
		{
			return d3d::to_dx12(cpu_view_heap(ResourceViewType::Texture2D))->create_texture2d_srv(texture.Get());
		}
		// Create a constant buffer view pointing for a given resource
		DxResourceView DirectXInterface::create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size)
		{
			return d3d::to_dx12(cpu_view_heap(ResourceViewType::ConstantBuffer))->create_cbv(resource.Get(), size);
		}
		DxResourceView DirectXInterface::create_dsv(const wrl::ComPtr<ID3D12Resource>& resource)
		{
			return d3d::to_dx12(cpu_view_heap(ResourceViewType::DepthStencil))->create_dsv(resource.Get());
		}
		DxResourceView DirectXInterface::create_uav(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size)
		{
			return d3d::to_dx12(cpu_view_heap(ResourceViewType::UnorderedAccess))->create_uav(resource.Get(), size);
		}

		rsl::unique_ptr<DxSampler2D> DirectXInterface::allocate_sampler2d(const SamplerDesc& desc)
		{
			return d3d::to_dx12(cpu_view_heap(ResourceViewType::Sampler))->create_sampler2d(desc);
		}

		// Initialize the various sub engines
		rsl::unique_ptr<RenderEngine> DirectXInterface::init_render_engine(ResourceStateTracker* resourceStateTracker)
		{
			return rsl::make_unique<DxRenderEngine>(resourceStateTracker);
		}
		rsl::unique_ptr<ComputeEngine> DirectXInterface::init_compute_engine(ResourceStateTracker* resourceStateTracker)
		{
			return rsl::make_unique<DxComputeEngine>(resourceStateTracker);
		}

		rsl::unique_ptr<DebugInterface> DirectXInterface::allocate_debug_interface()
		{
			return rsl::make_unique<DxDebugInterface>();
		}

		// Initialize the resource heap which keeps track of all gpu resources
		void DirectXInterface::init_resource_heap()
		{
			rsl::memory_size resource_heap_size = 100_mib; // hardcoded for now, will become a setting in the future
			CD3DX12_HEAP_DESC desc(resource_heap_size, D3D12_HEAP_TYPE_DEFAULT);

			wrl::ComPtr<ID3D12Heap> d3d_heap;
			if (DX_FAILED(m_device->dx_object()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
			{
				REX_ERROR(LogDxRhi, "Failed to create global resource heap");
			}

			m_heap = rsl::make_unique<DxResourceHeap>(d3d_heap, m_device->dx_object());
		}

		// Allocate a new view heap of a given type
		rsl::unique_ptr<ViewHeap> DirectXInterface::allocate_view_heap(ResourceViewType viewHeapType, IsShaderVisible isShaderVisible)
		{
			return create_view_heap(d3d::to_dx12(viewHeapType), isShaderVisible);
		}
	}
}