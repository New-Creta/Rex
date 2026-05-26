#include "pokemon/original_code/home/overworld.h"

#include "rex_engine/memory/pointer_math.h"

#include "pokemon/wram.h"

namespace pokemon
{
	void Overworld::loop()
	{
		delay_frame();
		overworld_loop_less_delay();
	}

	void Overworld::delay_frame()
	{
		// Wait for vsync
	}

	void Overworld::load_gb_pal()
	{
		// Loads the game boy pallete, setting up the colors mapped to values
	}

	void Overworld::move_ahead()
	{
		if (rsl::has_flag(wram.main_data.wd736, PlayerMovementFlags::IsSpinning))
		{
			load_spinner_arrow_tiles();
		}

		update_sprites();
	}

	void Overworld::load_spinner_arrow_tiles()
	{

	}

	void Overworld::run_map_script()
	{
		try_pushing_boulder();
	}


	void Overworld::joypad_overworld()
	{
		wram.sprite_state_data.player_sprite_data1.y_screen_pos_delta = 0;
		wram.sprite_state_data.player_sprite_data1.x_screen_position_delta = 0;

		run_map_script();

	}


	char Overworld::get_value_from_somewhere(rsl::string_view name)
	{
		return 0; // example value
	}

	char* Overworld::get_ptr_from_somewhere(rsl::string_view name)
	{
		return nullptr;
	}

	short* Overworld::get_wptr_from_somewhere(rsl::string_view name)
	{
		return nullptr;
	}

	char Overworld::dereference(int ptr)
	{
		return 0;
	}
	char Overworld::dereference(rsl::string_view name)
	{
		return 0;
	}

	// ld name val
	void Overworld::set_value(rsl::string_view name, int val)
	{

	}

	// ld [name] val
	void Overworld::set_value_at(rsl::string_view name, int val)
	{

	}

	void Overworld::delay_frames(int numFrames)
	{
		while (numFrames)
		{
			delay_frame();
			--numFrames;
		}
	}

	void Overworld::delay3()
	{
		delay_frames(3);
	}

	void Overworld::update_sprite()
	{

	}

	void Overworld::update_current_sprite(short* hl, short* de, short* bc)
	{
		if (*hl == 1)
		{
			update_player_sprite();
		}
		else
		{
			update_non_player_sprite();
		}
	}

	void Overworld::update_sprites_impl()
	{
		short* sprite_state_data_1_ptr = get_wptr_from_somewhere("wSpriteStateData1");
		// increment the top 8 bits
		sprite_state_data_1_ptr += 0x0100;
		char sprite_state_data_2_image_base_offset = get_value_from_somewhere("SPRITESTATEDATA2_IMAGEBASEOFFSET");

		// loop starts here
		while (true)
		{
			char sprite_state_data_2_image_base_offset_absolute = sprite_state_data_2_image_base_offset;
			char sprite_state_data_2_image_base_offset_relative = sprite_state_data_2_image_base_offset - get_value_from_somewhere("SPRITESTATEDATA2_IMAGEBASEOFFSET");
			char current_sprite_offset = sprite_state_data_2_image_base_offset_relative;
			set_value_at("hCurrentSpriteOffset", current_sprite_offset);
			short& sprite_state_data_1 = *sprite_state_data_1_ptr;
			if (sprite_state_data_1 == 0)
			{
				// this is to check for over flow, in old GB it wrapped around, the following should never trigger
				char next_sprite_offset = sprite_state_data_2_image_base_offset_absolute + 0x10;
				if (next_sprite_offset == get_value_from_somewhere("SPRITESTATEDATA2_IMAGEBASEOFFSET"))
				{
					break;
				}

				continue;
			}
			short de;
			short bc = sprite_state_data_2_image_base_offset_relative;
			update_current_sprite(sprite_state_data_1_ptr, &de, &bc);
			sprite_state_data_2_image_base_offset_absolute += 0x10;

			sprite_state_data_2_image_base_offset_absolute += 0x10;
			if (sprite_state_data_2_image_base_offset_absolute == get_value_from_somewhere("SPRITESTATEDATA2_IMAGEBASEOFFSET"))
			{
				break;
			}
		}
	}

	void Overworld::update_sprites()
	{
		char* update_sprites_disable_counter = get_ptr_from_somewhere("wUpdateSpritesEnabled");
		*update_sprites_disable_counter--;
		if (*update_sprites_disable_counter)
		{
			return;
		}

		update_sprites_impl();
	}

	void Overworld::finished_jump()
	{
		if (wram.data.wWalkCounter != 0)
		{
			return;
		}

		update_sprites();
		delay3();
		hram.hJoyHeld = 0;
		hram.hJoyPressed = 0;
		hram.hJoyReleased = 0;
		wram.main_data.wPlayerJumpingYScreenCoordsIndex = 0;
		rsl::remove_flag(wram.main_data.wd736, 0b00100000);
		rsl::remove_flag(wram.main_data.wd730, Wd730::SimulateJoypadStates);
		wram.data.wJoyIgnore = 0;
	}

	void Overworld::handle_mid_jump()
	{
		s8 player_jumping_y_screen_coords_idx_copy = wram.main_data.wPlayerJumpingYScreenCoordsIndex;
		++player_jumping_y_screen_coords_idx_copy;
		if (player_jumping_y_screen_coords_idx_copy < 0x10) // only update if we're at tile (x, 16) or more
		{
			finished_jump();
			return;
		}

		wram.main_data.wPlayerJumpingYScreenCoordsIndex = player_jumping_y_screen_coords_idx_copy;
		s8 PlayerJumpingYScreenCoords[] = {
			0x38, 0x36, 0x34, 0x32, 0x31, 0x30, 0x30, 0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x38, 0x3C, 0x3C
		};
		wram.sprite_state_data.player_sprite_data1.y_screen_position = PlayerJumpingYScreenCoords[wram.main_data.wPlayerJumpingYScreenCoordsIndex];
	}

	void Overworld::overworld_loop_less_delay()
	{
		delay_frame();
		load_gb_pal();

		if (rsl::has_flag(wram.main_data.wd736, PlayerMovementFlags::IsJumpingDownLedgeOrFishing))
		{
			handle_mid_jump();
		}
		if (wram.data.wWalkCounter > 0)
		{
			move_ahead();
			return;
		}
		joypad_overworld();
		safari_zone_check();
		if (safari_zone_game_over())
		{
			wrap_found2();
			return;
		}
		short status_flags3 = game_status_flags3();
		if (status_flags3.has_flag(wrap_from_cur_script))
		{
			status_flags3.clear(wrap_from_cur_script);
			wrap_found2();
			return;
		}
		char status_flags6 = game_status_flags6();
		if (status_flags6.has_flag(fly_warp) || status_flags6.has_flag(dungeon_warp))
		{
			handle_fly_wrap_or_dungeon_warp();
		}
		char current_opponent = game_current_opponent();
		if (current_opponent != 0)
		{
			new_battle();
			return;
		}
		char status_flags5 = game_status_flags5();
		if (status_flags5.has_flag(scripted_movement_state))
		{
			char joy_held = game_joy_held();
			check_if_start_is_pressed();
		}
		else
		{
			char joy_pressed = game_joy_pressed();
			check_if_start_is_pressed();
		}
	}
}




#include <cstdint>
#include <cstring>

// -----------------------------
// Mock hardware / memory layer
// -----------------------------

uint8_t hLoadedROMBank;
uint8_t MBC1RomBank;

uint8_t wTilesetBank;

uint8_t* wCurrentTileBlockMapViewPointer;

uint8_t wTileMapBackup[0x400];

uint8_t wXBlockCoord;
uint8_t wYBlockCoord;

uint8_t wCurMapWidth;
uint8_t wCurMapHeight;

uint8_t wTilesetBlocksPtr_low;
uint8_t wTilesetBlocksPtr_high;

uint8_t wMapViewVRAMPointer[2];

uint8_t SCREEN_WIDTH = 20;
uint8_t SCREEN_HEIGHT = 18;

uint8_t MAP_BORDER = 2;

// -----------------------------
// Helpers
// -----------------------------

static inline uint16_t getHL(uint8_t low, uint8_t high)
{
	return (uint16_t(low) | (uint16_t(high) << 8));
}

static inline void setHL(uint16_t value, uint8_t& low, uint8_t& high)
{
	low = value & 0xFF;
	high = value >> 8;
}

// -----------------------------
// Tile block drawing
// -----------------------------

extern uint8_t* TilesetBlocksPtr;

// Equivalent of DrawTileBlock
void DrawTileBlock(uint8_t tileBlockID, uint8_t* dest)
{
	uint16_t base = (uint16_t)TilesetBlocksPtr;
	uint16_t offset = tileBlockID * 16;
	uint8_t* src = (uint8_t*)(base + offset);

	uint8_t* hl = dest;

	for (int row = 0; row < 4; row++)
	{
		for (int i = 0; i < 4; i++)
		{
			hl[i] = src[i];
		}

		src += 4;
		hl += SCREEN_WIDTH; // corresponds to $15 stride in ASM (tilemap pitch behavior)
	}
}

// -----------------------------
// Build visible map (LoadCurrentMapView)
// -----------------------------

void LoadCurrentMapView()
{
	uint8_t savedBank = hLoadedROMBank;

	hLoadedROMBank = wTilesetBank;
	MBC1RomBank = wTilesetBank;

	uint8_t* de = wCurrentTileBlockMapViewPointer;
	uint8_t* hl = wTileMapBackup;

	for (int row = 0; row < 5; row++)
	{
		uint8_t* rowDE = de;
		uint8_t* rowHL = hl;

		for (int col = 0; col < 6; col++)
		{
			uint8_t tileBlock = *de;
			DrawTileBlock(tileBlock, hl);

			hl += 4;
			de += 1;
		}

		// advance tile-block pointer to next row
		de = rowDE + (wCurMapWidth + MAP_BORDER * 2);

		// advance tile buffer pointer
		hl = rowHL + 0x60;
	}

	// -----------------------------
	// Adjust for sub-block offset
	// -----------------------------

	uint16_t hlPtr = (uint16_t)wTileMapBackup;

	if (wYBlockCoord)
		hlPtr += 0x30;

	if (wXBlockCoord)
		hlPtr += 0x02;

	uint8_t* src = (uint8_t*)hlPtr;
	uint8_t* dest = (uint8_t*)0x9800; // decoord(0,0)

	// copy visible screen
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			dest[x] = src[x];
		}

		src += SCREEN_WIDTH + 4;
		dest += SCREEN_WIDTH;
	}

	hLoadedROMBank = savedBank;
	MBC1RomBank = savedBank;
}

#include <cstdint>
#include <cstring>

// -----------------------------
// Mock hardware / memory layer
// -----------------------------

uint8_t hLoadedROMBank;
uint8_t MBC1RomBank;

uint8_t wTilesetBank;

uint8_t* wCurrentTileBlockMapViewPointer;

uint8_t wTileMapBackup[0x400];

uint8_t wXBlockCoord;
uint8_t wYBlockCoord;

uint8_t wCurMapWidth;
uint8_t wCurMapHeight;

uint8_t wTilesetBlocksPtr_low;
uint8_t wTilesetBlocksPtr_high;

uint8_t wMapViewVRAMPointer[2];

uint8_t SCREEN_WIDTH = 20;
uint8_t SCREEN_HEIGHT = 18;

uint8_t MAP_BORDER = 2;

// -----------------------------
// Helpers
// -----------------------------

static inline uint16_t getHL(uint8_t low, uint8_t high)
{
	return (uint16_t(low) | (uint16_t(high) << 8));
}

static inline void setHL(uint16_t value, uint8_t& low, uint8_t& high)
{
	low = value & 0xFF;
	high = value >> 8;
}

// -----------------------------
// Tile block drawing
// -----------------------------

extern uint8_t* TilesetBlocksPtr;

// Equivalent of DrawTileBlock
void DrawTileBlock(uint8_t tileBlockID, uint8_t* dest)
{
	uint16_t base = (uint16_t)TilesetBlocksPtr;
	uint16_t offset = tileBlockID * 16;
	uint8_t* src = (uint8_t*)(base + offset);

	uint8_t* hl = dest;

	for (int row = 0; row < 4; row++)
	{
		for (int i = 0; i < 4; i++)
		{
			hl[i] = src[i];
		}

		src += 4;
		hl += SCREEN_WIDTH; // corresponds to $15 stride in ASM (tilemap pitch behavior)
	}
}

// -----------------------------
// Build visible map (LoadCurrentMapView)
// -----------------------------

void LoadCurrentMapView()
{
	uint8_t savedBank = hLoadedROMBank;

	hLoadedROMBank = wTilesetBank;
	MBC1RomBank = wTilesetBank;

	uint8_t* de = wCurrentTileBlockMapViewPointer;
	uint8_t* hl = wTileMapBackup;

	for (int row = 0; row < 5; row++)
	{
		uint8_t* rowDE = de;
		uint8_t* rowHL = hl;

		for (int col = 0; col < 6; col++)
		{
			uint8_t tileBlock = *de;
			DrawTileBlock(tileBlock, hl);

			hl += 4;
			de += 1;
		}

		// advance tile-block pointer to next row
		de = rowDE + (wCurMapWidth + MAP_BORDER * 2);

		// advance tile buffer pointer
		hl = rowHL + 0x60;
	}

	// -----------------------------
	// Adjust for sub-block offset
	// -----------------------------

	uint16_t hlPtr = (uint16_t)wTileMapBackup;

	if (wYBlockCoord)
		hlPtr += 0x30;

	if (wXBlockCoord)
		hlPtr += 0x02;

	uint8_t* src = (uint8_t*)hlPtr;
	uint8_t* dest = (uint8_t*)0x9800; // decoord(0,0)

	// copy visible screen
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			dest[x] = src[x];
		}

		src += SCREEN_WIDTH + 4;
		dest += SCREEN_WIDTH;
	}

	hLoadedROMBank = savedBank;
	MBC1RomBank = savedBank;
}

// =====================================================
// PART 3 — TILE BLOCK POINTER MOVEMENT + REDRAW SYSTEM
// (Map streaming + VRAM scheduling helpers)
// =====================================================

#include <cstdint>

// -----------------------------
// External RAM / state
// -----------------------------

uint8_t wCurrentTileBlockMapViewPointer[2];
uint8_t wCurMapWidth;

uint8_t wMapViewVRAMPointer[2];

uint8_t wRedrawRowOrColumnSrcTiles[0x200];

uint8_t hRedrawRowOrColumnDest[2];
uint8_t hRedrawRowOrColumnMode;

uint8_t REDRAW_ROW = 1;
uint8_t REDRAW_COL = 2;

uint8_t SCREEN_WIDTH = 20;
uint8_t SCREEN_HEIGHT = 18;

uint8_t MAP_BORDER = 2;

// =====================================================
// TILE BLOCK MAP POINTER MOVEMENT (streaming world)
// =====================================================

static inline void add16(uint8_t* ptr, uint16_t val)
{
	uint16_t v = ptr[0] | (ptr[1] << 8);
	v += val;
	ptr[0] = v & 0xFF;
	ptr[1] = v >> 8;
}

static inline void sub16(uint8_t* ptr, uint16_t val)
{
	uint16_t v = ptr[0] | (ptr[1] << 8);
	v -= val;
	ptr[0] = v & 0xFF;
	ptr[1] = v >> 8;
}

// -----------------------------
// Move east/west
// -----------------------------

void MoveTileBlockMapPointerEast(uint8_t* de)
{
	uint16_t v = wCurrentTileBlockMapViewPointer[0]
		| (wCurrentTileBlockMapViewPointer[1] << 8);

	v += 1;

	wCurrentTileBlockMapViewPointer[0] = v & 0xFF;
	wCurrentTileBlockMapViewPointer[1] = v >> 8;
}

void MoveTileBlockMapPointerWest(uint8_t* de)
{
	uint16_t v = wCurrentTileBlockMapViewPointer[0]
		| (wCurrentTileBlockMapViewPointer[1] << 8);

	v -= 1;

	wCurrentTileBlockMapViewPointer[0] = v & 0xFF;
	wCurrentTileBlockMapViewPointer[1] = v >> 8;
}

// -----------------------------
// Move south/north (row jump)
// -----------------------------

void MoveTileBlockMapPointerSouth(uint8_t* de, uint8_t mapWidth)
{
	uint16_t stride = mapWidth + (MAP_BORDER * 2);

	uint16_t v = wCurrentTileBlockMapViewPointer[0]
		| (wCurrentTileBlockMapViewPointer[1] << 8);

	v += stride;

	wCurrentTileBlockMapViewPointer[0] = v & 0xFF;
	wCurrentTileBlockMapViewPointer[1] = v >> 8;
}

void MoveTileBlockMapPointerNorth(uint8_t* de, uint8_t mapWidth)
{
	uint16_t stride = mapWidth + (MAP_BORDER * 2);

	uint16_t v = wCurrentTileBlockMapViewPointer[0]
		| (wCurrentTileBlockMapViewPointer[1] << 8);

	v -= stride;

	wCurrentTileBlockMapViewPointer[0] = v & 0xFF;
	wCurrentTileBlockMapViewPointer[1] = v >> 8;
}

// =====================================================
// REDRAW SCHEDULING SYSTEM (VBlank-driven streaming)
// =====================================================

// copy 2 rows of tiles into buffer
void CopyToRedrawRowOrColumnSrcTiles(uint8_t* hl)
{
	uint8_t* de = wRedrawRowOrColumnSrcTiles;

	for (int i = 0; i < SCREEN_WIDTH * 2; i++)
	{
		*de++ = *hl++;
	}
}

// -----------------------------
// North row redraw
// -----------------------------

void ScheduleNorthRowRedraw()
{
	uint8_t* hl = &wMapViewVRAMPointer[0];

	CopyToRedrawRowOrColumnSrcTiles(hl);

	hRedrawRowOrColumnDest[0] = wMapViewVRAMPointer[0];
	hRedrawRowOrColumnDest[1] = wMapViewVRAMPointer[1];

	hRedrawRowOrColumnMode = REDRAW_ROW;
}

// -----------------------------
// South row redraw
// -----------------------------

void ScheduleSouthRowRedraw()
{
	uint8_t* hl = &wMapViewVRAMPointer[0];

	uint16_t v = hl[0] | (hl[1] << 8);
	v += 0x200; // move down 2 rows

	hRedrawRowOrColumnDest[0] = v & 0xFF;
	hRedrawRowOrColumnDest[1] = (v >> 8) & 0xFF;

	hRedrawRowOrColumnMode = REDRAW_ROW;
}

// =====================================================
// COLUMN REDRAW HELPERS
// =====================================================

void ScheduleColumnRedrawHelper(uint8_t* hl)
{
	uint8_t* de = wRedrawRowOrColumnSrcTiles;

	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		de[0] = hl[0];
		de[1] = hl[1];

		de += 2;
		hl += SCREEN_WIDTH + 1;
	}
}

// -----------------------------
// East column redraw
// -----------------------------

void ScheduleEastColumnRedraw()
{
	uint8_t* hl = &wMapViewVRAMPointer[0];

	ScheduleColumnRedrawHelper(hl);

	uint16_t v = hl[0] | (hl[1] << 8);
	v += 18;

	hRedrawRowOrColumnDest[0] = v & 0xFF;
	hRedrawRowOrColumnDest[1] = v >> 8;

	hRedrawRowOrColumnMode = REDRAW_COL;
}

// -----------------------------
// West column redraw
// -----------------------------

void ScheduleWestColumnRedraw()
{
	uint8_t* hl = &wMapViewVRAMPointer[0];

	ScheduleColumnRedrawHelper(hl);

	hRedrawRowOrColumnDest[0] = wMapViewVRAMPointer[0];
	hRedrawRowOrColumnDest[1] = wMapViewVRAMPointer[1];

	hRedrawRowOrColumnMode = REDRAW_COL;
}

// =====================================================
// PART 4 — JOYPAD + INPUT SIMULATION SYSTEM
// (JoypadOverworld + forced movement + scripted input)
// =====================================================

#include <cstdint>

// -----------------------------
// Input state (mock RAM / HW)
// -----------------------------

uint8_t hJoyHeld;
uint8_t hJoyPressed;
uint8_t hJoyReleased;

uint8_t wSpritePlayerStateData1YStepVector;
uint8_t wSpritePlayerStateData1XStepVector;

uint8_t wFlags_D733;
uint8_t wCurMap;

uint8_t wd730;
uint8_t wd732;
uint8_t wd736;

uint8_t wOverrideSimulatedJoypadStatesMask;

uint8_t wSimulatedJoypadStatesIndex;
uint8_t wSimulatedJoypadStatesEnd;

uint8_t wJoyIgnore;

uint8_t wUnusedCD3A;

// -----------------------------
// Direction constants
// -----------------------------

constexpr uint8_t D_UP = 0x04;
constexpr uint8_t D_DOWN = 0x08;
constexpr uint8_t D_LEFT = 0x02;
constexpr uint8_t D_RIGHT = 0x01;
constexpr uint8_t A_BUTTON = 0x80;
constexpr uint8_t B_BUTTON = 0x40;
constexpr uint8_t START = 0x20;
constexpr uint8_t SELECT = 0x10;

// -----------------------------
// External functions
// -----------------------------

void RunMapScript();
void Joypad();

// =====================================================
// JOYPAD OVERWORLD MAIN LOGIC
// =====================================================

void JoypadOverworld()
{
	wSpritePlayerStateData1YStepVector = 0;
	wSpritePlayerStateData1XStepVector = 0;

	RunMapScript();
	Joypad();

	// ---------------------------------------------
	// Cycling Road forced movement logic
	// ---------------------------------------------

	if (!(wFlags_D733 & (1 << 3)))
	{
		if (wCurMap == 0x00 /* ROUTE_17 */)
		{
			if ((hJoyHeld & (D_DOWN | D_UP | D_LEFT | D_RIGHT | A_BUTTON | B_BUTTON)) == 0)
			{
				hJoyHeld = D_DOWN;
			}
		}
	}

	// ---------------------------------------------
	// input simulation active?
	// ---------------------------------------------

	if (!(wd730 & (1 << 7)))
		return;

	uint8_t held = hJoyHeld;

	if (wOverrideSimulatedJoypadStatesMask & held)
		return;

	// ---------------------------------------------
	// scripted input playback
	// ---------------------------------------------

	if (wSimulatedJoypadStatesIndex == 0xFF)
	{
		// finished simulation
		wUnusedCD3A = 0;
		wSimulatedJoypadStatesIndex = 0;
		wSimulatedJoypadStatesEnd = 0;
		wJoyIgnore = 0;

		hJoyHeld = 0;
		hJoyPressed = 0;
		hJoyReleased = 0;

		wd736 &= 0xF8;
		wd730 &= ~(1 << 7);

		return;
	}

	uint8_t index = --wSimulatedJoypadStatesIndex;

	uint8_t* script = &wSimulatedJoypadStatesEnd; // placeholder base

	uint8_t value = script[index];

	hJoyHeld = value;

	if (value == 0)
	{
		hJoyPressed = 0;
		hJoyReleased = 0;
	}
}

// =====================================================
// INPUT INTERRUPTION CHECK (intro/title screen)
// =====================================================

bool CheckForUserInterruption(uint8_t frames)
{
	while (frames--)
	{
		Joypad();

		uint8_t held = hJoyHeld;

		if (held == (D_UP | SELECT | B_BUTTON))
			return true;

		uint8_t held5 = hJoyPressed;

		if (held5 & (START | A_BUTTON))
			return true;
	}

	return false;
}

// =====================================================
// FORCE MOVEMENT / BIKE / SURF SWITCH
// =====================================================

void ForceBikeOrSurf()
{
	LoadPlayerSpriteGraphics(); // banked call in original
	PlayDefaultMusic();
}

// =========================
// Overworld (remaining tail)
// C++ translation (pseudo-engine style)
// =========================

#include <cstdint>

extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

extern uint8_t wPredefParentBank;

extern uint8_t wCurrentTileBlockMapViewPointer[2];

// -------------------------------------------------
// CopyData helper (assumed engine primitive)
// -------------------------------------------------
void CopyData(uint8_t* src, uint8_t* dst, uint8_t size);

// -------------------------------------------------
// LoadDestinationWarpPosition
// -------------------------------------------------
void LoadDestinationWarpPosition(uint8_t warpId, uint8_t* hlSrc)
{
	uint8_t savedBank = hLoadedROMBank;

	hLoadedROMBank = wPredefParentBank;
	MBC1RomBank = wPredefParentBank;

	uint8_t idx = warpId * 4;

	uint8_t* src = hlSrc + idx;
	uint8_t* dst = wCurrentTileBlockMapViewPointer;

	CopyData(src, dst, 4);

	hLoadedROMBank = savedBank;
	MBC1RomBank = savedBank;
}

// -------------------------------------------------
// CheckForUserInterruption
// -------------------------------------------------
bool CheckForUserInterruption(uint8_t frames)
{
	while (frames--)
	{
		DelayFrame();
		JoypadLowSensitivity();

		uint8_t held = hJoyHeld;

		if (held == (D_UP | SELECT | B_BUTTON))
			return true;

#ifdef _DEBUG
		if (hJoy5 & (START | SELECT | A_BUTTON))
			return true;
#else
		if (hJoy5 & (START | A_BUTTON))
			return true;
#endif
	}

	return false;
}

// -------------------------------------------------
// ForceBikeOrSurf
// -------------------------------------------------
void ForceBikeOrSurf()
{
	uint8_t bank = BANK(RedSprite);

	Bankswitch(bank, LoadPlayerSpriteGraphics);

	PlayDefaultMusic();
}

// -------------------------------------------------
// ResetUsingStrengthOutOfBattleBit
// -------------------------------------------------
void ResetUsingStrengthOutOfBattleBit()
{
	wd728 &= ~(1 << 0);
}

// -------------------------------------------------
// IgnoreInputForHalfSecond
// -------------------------------------------------
void IgnoreInputForHalfSecond()
{
	wIgnoreInputCounter = 30;
	wd730 |= 0x26;
}

// -------------------------------------------------
// SwitchToMapRomBank
// -------------------------------------------------
void SwitchToMapRomBank(uint8_t mapId)
{
	uint8_t bank = BankswitchHome(BANK(MapHeaderBanks));

	uint8_t mapBank = MapHeaderBanks[mapId];

	hMapROMBank = mapBank;

	BankswitchBack(bank);

	hLoadedROMBank = mapBank;
	MBC1RomBank = mapBank;
}

// -------------------------------------------------
// LoadMapData (wrapper style conversion stub)
// -------------------------------------------------
void LoadMapData()
{
	uint8_t savedBank = hLoadedROMBank;

	DisableLCD();

	wMapViewVRAMPointer[1] = 0x98;
	wMapViewVRAMPointer[0] = 0;

	hSCY = 0;
	hSCX = 0;

	wWalkCounter = 0;
	wUnusedD119 = 0;
	wWalkBikeSurfStateCopy = 0;
	wSpriteSetID = 0;

	LoadTextBoxTilePatterns();

	LoadMapHeader();
	InitMapSprites();

	LoadTileBlockMap();
	LoadTilesetTilePatternData();

	LoadCurrentMapView();

	CopyToVRAM_BGMap();

	wUpdateSpritesEnabled = 1;

	EnableLCD();

	RunPaletteCommand(SET_PAL_OVERWORLD);

	LoadPlayerSpriteGraphics();

	if ((wd732 & (1 << 4)) || (wd732 & (1 << 3)))
	{
		hLoadedROMBank = savedBank;
		MBC1RomBank = savedBank;
		return;
	}

	if (wFlags_D733 & (1 << 1))
	{
		hLoadedROMBank = savedBank;
		MBC1RomBank = savedBank;
		return;
	}

	UpdateMusic6Times();
	PlayDefaultMusicFadeOutCurrent();

	hLoadedROMBank = savedBank;
	MBC1RomBank = savedBank;
}

// -------------------------------------------------
// CopyMapConnectionHeader
// -------------------------------------------------
void CopyMapConnectionHeader(uint8_t* src, uint8_t* dst)
{
	for (int i = 0; i < 0x0B; i++)
		dst[i] = src[i];
}

// -------------------------------------------------
// LoadMapHeader (final portion stubbed full translation)
// -------------------------------------------------
void LoadMapHeader()
{
	MarkTownVisitedAndLoadMissableObjects();

	wUnusedD119 = wCurMapTileset;

	SwitchToMapRomBank(wCurMap);

	uint8_t tileset = wCurMapTileset & ~0x80;
	wCurMapTileset = tileset;
	hPreviousTileset = tileset;

	if (wCurMapTileset & 0x80)
		return;

	MapHeader* header = MapHeaderPointers[wCurMap];

	memcpy(&wCurMapHeader, header, sizeof(CurMapHeader));

	wNorthConnectedMap = 0xFF;
	wSouthConnectedMap = 0xFF;
	wWestConnectedMap = 0xFF;
	wEastConnectedMap = 0xFF;

	uint8_t conn = wCurMapConnections;

	if (conn & NORTH_F) CopyMapConnectionHeader(...);
	if (conn & SOUTH_F) CopyMapConnectionHeader(...);
	if (conn & WEST_F)  CopyMapConnectionHeader(...);
	if (conn & EAST_F)  CopyMapConnectionHeader(...);

	uint8_t* obj = wObjectDataPointerTemp;

	wMapBackgroundTile = obj[0];

	uint8_t warpCount = obj[1];
	wNumberOfWarps = warpCount;

	// (warp/sign/sprite parsing omitted here due to engine dependency stubs)

	LoadTilesetHeader();
	LoadWildData();

	wCurrentMapHeight2 = wCurMapHeight * 2;
	wCurrentMapWidth2 = wCurMapWidth * 2;

	uint8_t map = wCurMap;
	uint8_t songBank = MapSongBanks[map].bank;
	uint8_t songId = MapSongBanks[map].song;

	wMapMusicROMBank = songBank;
	wMapMusicSoundID = songId;
}

// -------------------------------------------------
// END OF FILE CONVERSION
// -------------------------------------------------