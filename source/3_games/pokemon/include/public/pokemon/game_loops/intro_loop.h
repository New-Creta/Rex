#pragma once

#include "pokemon/game_loops/game_loop.h"
#include "pokemon/state_task.h"

#include "rex_std/unordered_map.h"

namespace pokemon
{
	class IntroLoop : public GameLoop
	{
		enum class State
		{
			Copyright,
			GamefreakLogo,
			Fight,
			StartMenu
		};

	public:
		IntroLoop();
		rsl::unique_ptr<GameLoop> run() override;

	private:
		// Copyright
		void begin_copyright();
		void tick_copyright();
		void end_copyright();

		// Gamefreak logo
		void begin_gamefreak();
		void tick_gamefreak();
		void end_gamefreak();

		// Fight Scene
		void begin_fight_scene();
		void tick_fight_scene();
		void end_fight_scene();

		// Start Menu
		void begin_start_menu();
		void tick_start_menu();
		void end_start_menu();



		void display_copyright();
		void display_gamefreak_logo();
		void display_fight();
		void display_start_menu();

	private:
		State m_state;
		rsl::unordered_map<State, StateTask> m_state_tree;

		GameLoop m_display_copyright;

		GameLoop* m_active_loop;
	};
}