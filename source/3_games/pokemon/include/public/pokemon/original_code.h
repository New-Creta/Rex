#pragma once

#include "rex_std/string_view.h"

void delay_frame()
{
	// Wait for vsync
}

void overworld_loop()
{
	delay_frame();
	overworld_loop_less_delay();
}

void load_gb_pal()
{
	// Loads the game boy pallete, setting up the colors mapped to values
}

char get_value_from_somewhere(rsl::string_view name)
{
	return 0; // example value
}

char* get_ptr_from_somewhere(rsl::string_view name)
{
	return nullptr;
}

short* get_wptr_from_somewhere(rsl::string_view name)
{
	return nullptr;
}

char dereference(int ptr)
{
	return 0;
}
char dereference(rsl::string_view name)
{
	return 0;
}

// ld name val
void set_value(rsl::string_view name, int val)
{
	
}

// ld [name] val
void set_value_at(rsl::string_view name, int val)
{

}

void delay_frames(int numFrames)
{
	while (numFrames)
	{
		delay_frame();
		--numFrames;
	}
}

void delay3()
{
	delay_frames(3);
}

void update_sprite()
{

}

void update_current_sprite(short* hl, short* de, short* bc)
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

void update_sprites_impl()
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

void update_sprites()
{
	char* update_sprites_disable_counter = get_ptr_from_somewhere("wUpdateSpritesEnabled");
	*update_sprites_disable_counter--;
	if (*update_sprites_disable_counter)
	{
		return;
	}

	update_sprites_impl();
}

void finished_jump()
{
	char walk_counter = get_value_from_somewhere();
	if (walk_counter != 0)
	{
		return;
	}

	update_sprites();
	delay3();
	set_value("hJoyHeld", 0);
	set_value("hJoyPressed", 0);
	set_value("hJoyReleased", 0);
	set_value("wPlayerJumpingYScreenCoordsIndex", 0);
	char* player_movement_flags = get_ptr_from_somewhere("wMovementFlags");
	player_movement_flags->clear_flag(LedgeOrFishing);
	char* game_status_flags5 = get_ptr_from_somewhere("wStatusFlags5");
	game_status_flags5->clear_flag(ScriptedMovementState);
	set_value("wJoyIgnore", 0);
}

void handle_mid_jump()
{
	char player_jumping_y_screen_coords_idx = get_value_from_somewhere("wPlayerJumpingYScreenCoordsIndex");
	char player_jumping_y_screen_coords_idx_copy = player_jumping_y_screen_coords_idx;
	++player_jumping_y_screen_coords_idx;
	if (player_jumping_y_screen_coords_idx < 0x10) // only update if we're at tile (x, 16) or more
	{
		finished_jump();
		return;
	}
	set_value_at("player_jumping_y_screen_coords_idx", player_jumping_y_screen_coords_idx);
	char* player_jumping_screen_coords = get_ptr_from_somewhere("PlayerJumpingYScreenCoords");
	player_jumping_screen_coords = &player_jumping_screen_coords[player_jumping_y_screen_coords_idx];
	set_value_at("wSpritePlayerStateData1YPixels", *player_jumping_screen_coords);
}

void overworld_loop_less_delay()
{
	delay_frame();
	load_gb_pal();
	char movement_flags = player_movement_flags();
	if (movement_flags.has_flag(LedgeOrFishing))
	{
		handle_mid_jump();
	}
	if (walk_counter() > 0)
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