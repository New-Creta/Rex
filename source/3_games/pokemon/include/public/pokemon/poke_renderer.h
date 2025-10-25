//#pragma once
//
//#include "rex_engine/gfx/rendering/renderer.h"
//
//#include "rex_engine/gfx/rendering/render_passes/block_pass.h"
//
//namespace rex
//{
//	class TilesetAsset;
//
//	namespace gfx
//	{
//		class BlockRenderPass;
//		class AnimatedSpritePass;
//	}
//}
//
//namespace pokemon
//{
//	struct PokemonRendererParams
//	{
//		rex::gfx::BlockRenderPassUpdateParams blockpass_params;
//	};
//
//	class PokemonRenderer : public rex::gfx::Renderer
//	{
//	public:
//		PokemonRenderer(const rex::TilesetAsset* tileset);
//
//		void update_params(const PokemonRendererParams& params);
//
//	private:
//		rex::gfx::BlockRenderPass* m_block_render_pass;
//		rex::gfx::AnimatedSpritesPass* m_animted_sprites_pass;
//	};
//}