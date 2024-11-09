//#pragma once
//
//#include "rex_std/string_view.h"
//#include "rex_engine/gfx/scenegraph/scene.h"
//#include "rex_std/unordered_map.h"
//#include "pokemon/chunk.h"
//
//#include "rex_engine/entities/entity.h"
//
//#include "rex_engine/text_processing/json.h"
//#include "pokemon/chunk_component.h"
//#include "pokemon/tileset.h"
//#include "pokemon/blockset.h"
//#include "pokemon/map_blocks.h"
//
//#include "rex_std/bonus/math.h"
//
//#include "glm/glm.hpp"
//
//namespace rex
//{
//  namespace gfx
//  {
//    class Scene;
//  }
//}
//
//namespace pokemon
//{
//  struct MapConnection
//  {
//    rsl::string map;
//    Direction direction;
//    s32 offset;
//  };
//
//  // A map is a section for the in game level
//  // a game consists of multiple levels
//  // a level consists of 1 or more maps
//  // for menus, the map is often empty
//  // for the actual game, the map is filled with objects
//  // In pokemon's case, a map is an area like Johto, Kanto, Hoenn
//
//  // Maps cannot have chunks neigbouring other map's chunks
//  // A transfer from one map to another should always be a hard transfer
//  // often done through a black screen, animation, or a special map connector
//  // Which is an area not tied to a map but shared between maps it connects to
//  class Map
//  {
//  public: 
//    Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob);
//
//    MapConnection* north_connection();
//    MapConnection* east_connection();
//    MapConnection* south_connection();
//    MapConnection* west_connection();
//
//    // the player position is always relative to the top left of the map
//    void load_viewable_blocks(rsl::pointi8 playerPosition);
//
//    // Load the player into a particular chunk with a given name
//    void set_player_chunk(rsl::string_view name);
//
//  private:
//    void load_all_tilesets(rsl::string_view tilesetsPath);
//
//    void load_all_chunks(const rex::json::json& jsonBlob);
//    bool is_current_player_chunk(rsl::string_view name) const;
//
//    rsl::unique_array<rsl::byte> allocate_map_texture_memory(const rex::json::json& jsonblob);
//    void fill_texture_with_padding_tile(const rex::json::json& jsonBlob, rsl::byte* textureMemory, s32 numTiles);
//    void fill_texture_with_map(const rex::json::json& jsonBlob, rsl::byte* textureMemory);
//    void fill_texture_with_connections(const rex::json::json& jsonBlob, rsl::byte* textureMemory);
//
//  private:
//    rsl::unique_ptr<MapConnection> m_north_connection;
//    rsl::unique_ptr<MapConnection> m_east_connection;
//    rsl::unique_ptr<MapConnection> m_south_connection;
//    rsl::unique_ptr<MapConnection> m_west_connection;
//
//    rsl::shared_ptr<TileSet> m_tileset;
//    rsl::shared_ptr<BlockSet> m_blockset;
//    rsl::shared_ptr<MapBlocks> m_blocks;
//    s8 m_width;
//    s8 m_height;
//
//    rsl::string m_name;
//    rex::gfx::Scene* m_scene;
//    rsl::vector<rex::gfx::Entity> m_chunks;
//    rex::gfx::Entity m_player_chunk;
//    rsl::vector<ChunkComponent*> m_active_chunk_components;
//    rsl::vector<rsl::unique_ptr<TileSet>> m_tilesets;
//
//  };
//}