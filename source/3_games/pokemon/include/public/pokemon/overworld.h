#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

namespace pokemon
{
	class Overworld
	{
	public:
		void loop();

	private:
		void delay_frame();
		void load_gb_pal();

		void move_ahead();
		void load_spinner_arrow_tiles();

		char get_value_from_somewhere(rsl::string_view name);

		char* get_ptr_from_somewhere(rsl::string_view name);

		short* get_wptr_from_somewhere(rsl::string_view name);

		char dereference(int ptr);
		char dereference(rsl::string_view name);

		// ld name val
		void set_value(rsl::string_view name, int val);

		// ld [name] val
		void set_value_at(rsl::string_view name, int val);

		void delay_frames(int numFrames);

		void delay3();

		void update_sprite();

		void update_current_sprite(short* hl, short* de, short* bc);

		void update_sprites_impl();

		void update_sprites();

		void finished_jump();

		void handle_mid_jump();

		void overworld_loop_less_delay();
	};
}