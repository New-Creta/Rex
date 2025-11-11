#pragma once

#include "rex_engine/gfx/rendering/render_pass.h"

#include "rex_engine/assets/texture_asset.h"

namespace rex
{
	namespace gfx
	{
		struct PerFrameData
		{
			glm::vec2 inv_tile_size_on_screen; // The inverse dimensions of the tile on the backbuffer
		};

		struct PerTextureData
		{
			rsl::pointi8 sprite_size;   // size of a single tile, in pixels
		};

		struct PerSpriteInstanceData
		{
			// Information needed to draw the sprite at the correct position on screen
			s32 tile_index_on_screen;   

			// Information needed to select the correct sprite in the texture
			s32 sprite_idx_in_texture;	// which sprite idx to use for the animation
			bool flip_x;								// flip the sprite, left becomes right, right becomes left
		};

		struct AnimationRenderData
		{
			rex::TilesetAsset texture; // texture that has the sprites to use for the animation
			s32 idx;										// which sprite idx to use for the animation
			bool flip_x;								// flip the sprite, left becomes right, right becomes left
		};

		struct AnimatedSpritesPassDynamicInputs
		{
			RenderTargetBase* render_target;
			rsl::point<TileCount> screen_resolution;
		};

		struct AnimatedSpriteDrawList
		{
			const TilesetAsset* tileset;// the tileset holding the sprite
			rsl::pointi8 top_left;
			s16 sprite_idx;							// which sprite idx to use for the animation
			bool flip_x;								// flip the sprite, left becomes right, right becomes left
			bool flip_y;								// flip the sprite, up becomes down, down becomes up
		};

		struct PerInstanceCB
		{
			u32 texture_tiles_per_row;   // the number of tiles per row in the tileset texture
			f32 inv_texture_width;       // the inverse width of the tileset texture, in pixels
			f32 inv_texture_height;      // the inverse height of the tileset texture, in pixels

			s16 sprite_idx;
			bool flip_x;
			bool flip_y;
		};

		struct AnimatedSpritePassCreationInfo
		{
			RenderTargetBase* render_target;
		};
		struct AnimatedSpritePassSceneParams
		{
			const TilesetAsset* tileset;
		};
		struct AnimatedSpritePassCameraParams
		{
			rsl::point<f32> zoom_level = rsl::point<f32>(1.0f, 1.0f);
		};


		class AnimatedSpritesPass : public RenderPass
		{
		public:
			AnimatedSpritesPass(const AnimatedSpritePassCreationInfo& creationInfo);

			void update_scene_params(const SceneParams& params);
			void update_camera_params(const CameraParams& params);

			AnimatedSprite* add_sprite(rsl::unique_ptr<AnimatedSprite> sprite);

			void push_sprite(const AnimatedSpriteDrawList& drawlist);
			void render(rex::gfx::RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc(const AnimatedSpritePassCreationInfo& creationInfo) const;

			void init();

			void init_vb(rex::gfx::RenderContext* renderCtx);
			void init_render_info(rex::gfx::RenderContext* renderCtx);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			void init_tile_indices_uab();
			void init_shader_params();

		private:
			// The vertex buffer for a single tile
			// As we use instanced rendering, we only store 4 vertices
			rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
			// The index buffer for a single tile
			// It's just 6 indices, allowing us to make a single quad
			rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;

			// The render info the shader needs for rendering
			// It's to store how big a single tile is, both on the tilemap 
			// as well as in the texture
			rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_render_info;
			rsl::unique_ptr<rex::gfx::ConstantBuffer> m_per_instance_cb;

			// The UAV holding the indices of all the tiles to render
			// starting from the top left, going to the bottom right
			rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_per_instance_info;

			rex::gfx::RenderTargetBase* m_render_target;

			rsl::vector<AnimatedSpriteDrawList> m_draw_list;

			rsl::vector<rsl::unique_ptr<AnimatedSprite>> m_sprites;

			SceneParams m_scene_params;
			CameraParams m_camera_params;
		};
	}
}