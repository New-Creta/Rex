#pragma once

#include "pokemon/game_loops/game_loop.h"

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
		void display_copyright();
		void display_gamefreak_logo();
		void display_fight();
		void display_start_menu();

	private:
		State m_state;

		GameLoop m_display_copyright;

		GameLoop* m_active_loop;
	};
}