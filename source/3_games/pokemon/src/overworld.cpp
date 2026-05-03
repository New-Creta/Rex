#include "pokemon/overworld.h"

namespace pokemon
{
	struct SpriteStateData1
	{
		s8 picture_id;											// - 0: picture ID (fixed, loaded at map init)
		s8 movement_status;									// - 1: movement status (0: uninitialized, 1: ready, 2: delayed, 3: moving)
		s8 image_idx;												// - 2: sprite image index (changed on update, $ff if off screen, includes facing direction, progress in walking animation and a sprite-specific offset)
		s8 y_screen_pos_delta;							// - 3: Y screen position delta (-1,0 or 1; added to Y pixels on each walking animation update)
		s8 y_screen_position;								// - 4: Y screen position (in pixels, always 4 pixels above grid which makes sprites appear to be in the center of a tile)
		s8 x_screen_position_delta;					// - 5: X screen position delta (-1,0 or 1; added to field X pixels on each walking animation update)
		s8 x_screen_position;								// - 6: X screen position (in pixels, snaps to grid if not currently walking)
		s8 intra_animation_frame_counter;		// - 7: intra-animation-frame counter (counting upwards to 4 until animation frame counter is incremented)
		s8 animation_frame_counter;					// - 8: animation frame counter (increased every 4 updates, hold four states (totalling to 16 walking frames)
		s8 face_direction;									// - 9: facing direction ($0: down, $4: up, $8: left, $c: right)
		s8 adjusted_y_coordinate;						// - A: adjusted Y coordinate
		s8 adjusted_x_coordinate;						// - B: adjusted X coordinate
		s8 direction_of_collision;					// - C: direction of collision
		s8 unusued_1;
		s8 unusued_2;
		s8 unusued_3;
	};

	struct SpriteStateData2
	{
		s8 walk_animation_counter;			// - 0: walk animation counter (counting from $10 backwards when moving)
		s8 unused_1;										// - 1:
		s8 y_displacement;							// - 2: Y displacement (initialized at 8, supposed to keep moving sprites from moving too far, but bugged)
		s8 x_displacement;							// - 3: X displacement (initialized at 8, supposed to keep moving sprites from moving too far, but bugged)
		s8 y_position;									// - 4: Y position (in 2x2 tile grid steps, topmost 2x2 tile has value 4)
		s8 x_position;									// - 5: X position (in 2x2 tile grid steps, leftmost 2x2 tile has value 4)
		s8 movement_byte;								// - 6: movement byte 1 (determines whether a sprite can move, $ff:not moving, $fe:random movements, others unknown)
		s8 is_on_grass;									// - 7: (?) (set to $80 when in grass, else $0; may be used to draw grass above the sprite)
		s8 delay_until_next_movement;		// - 8: delay until next movement (counted downwards, movement status is set to ready if reached 0)
		s8 original_facing_direction;		// - 9: original facing direction (backed up by DisplayTextIDInit, restored by CloseTextDisplay)
		s8 unused_2;										// - A
		s8 unused_3;										// - B
		s8 unused_4;										// - C
		s8 picture_id;									// - D: picture ID
		s8 sprite_image_base_offset;		// - E: sprite image base offset (in video ram, player always has value 1, used to compute sprite image index)
		s8 unused_5;										// - F
	};

	enum class Wd730
	{
		// bit 0: NPC sprite being moved by script
		NpcSpriteMovedByScript = BIT(0),
		// bit 5: ignore joypad input
		IgnoreJoypadInput = BIT(5),
		// bit 6: print text with no delay between each letter
		PrintTextWithNoDelayBetweenLetters = BIT(6),
		// bit 7: set if joypad states are being simulated in the overworld or an NPC's movement is being scripted
		SimulateJoypadStates = BIT(7)
	};

	enum class PlayerMovementFlags : u8
	{
		// check if the player is standing on a door and make him walk down a step if so
		IsOnDoor												= BIT(0),

		// the player is currently stepping down from a door
		IsSteppingDownFromDoor					= BIT(1),
		
		// standing on a warp
		IsOnWarp												= BIT(2),
		
		// jumping down a ledge / fishing animation
		IsJumpingDownLedgeOrFishing			= BIT(6),
		
		// player sprite spinning due to spin tiles (Rocket hideout / Viridian Gym)
		IsSpinning											= BIT(7)
	};

	struct Wram
	{
		// Set buttons are ignored.
		s8 wJoyIgnore;

		Wd730 wd730;
		PlayerMovementFlags wd736;

		s8 wPlayerJumpingYScreenCoordsIndex;

		SpriteStateData1 player_sprite_data1;
		SpriteStateData2 player_sprite_data2;

		SpriteStateData1 sprite_data1[16];
		SpriteStateData2 sprite_data2[16];

		// walk animation counter
		s8 wWalkCounter;
	} wram;

	struct Hram
	{
		s8 hJoyLast;
		s8 hJoyReleased;
		s8 hJoyPressed;
		s8 hJoyHeld;
	} hram;


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
		if (rsl::has_flag(wram.wd736, PlayerMovementFlags::IsSpinning))
		{
			load_spinner_arrow_tiles();
		}

		update_sprites();
	}
	
	void Overworld::load_spinner_arrow_tiles()
	{

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
		if (wram.wWalkCounter != 0)
		{
			return;
		}

		update_sprites();
		delay3();
		hram.hJoyHeld = 0;
		hram.hJoyPressed = 0;
		hram.hJoyReleased = 0;
		wram.wPlayerJumpingYScreenCoordsIndex = 0;
		rsl::remove_flag(wram.wd736, 0b00100000);
		rsl::remove_flag(wram.wd730, Wd730::SimulateJoypadStates);
		wram.wJoyIgnore = 0;
	}

	void Overworld::handle_mid_jump()
	{
		s8 player_jumping_y_screen_coords_idx_copy = wram.wPlayerJumpingYScreenCoordsIndex;
		++player_jumping_y_screen_coords_idx_copy;
		if (player_jumping_y_screen_coords_idx_copy < 0x10) // only update if we're at tile (x, 16) or more
		{
			finished_jump();
			return;
		}

		wram.wPlayerJumpingYScreenCoordsIndex = player_jumping_y_screen_coords_idx_copy;
		s8 PlayerJumpingYScreenCoords[] = {
			0x38, 0x36, 0x34, 0x32, 0x31, 0x30, 0x30, 0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x38, 0x3C, 0x3C
		};
		wram.player_sprite_data1.y_screen_position = PlayerJumpingYScreenCoords[wram.wPlayerJumpingYScreenCoordsIndex];
	}

	void Overworld::overworld_loop_less_delay()
	{
		delay_frame();
		load_gb_pal();

		if (rsl::has_flag(wram.wd736, PlayerMovementFlags::IsJumpingDownLedgeOrFishing))
		{
			handle_mid_jump();
		}
		if (wram.wWalkCounter > 0)
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