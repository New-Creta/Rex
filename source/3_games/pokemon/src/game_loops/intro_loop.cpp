#include "pokemon/game_loops/intro_loop.h"

namespace pokemon
{
	IntroLoop::IntroLoop()
		: m_state(State::Copyright)
		, m_display_copyright()
		, m_active_loop(&m_display_copyright)
	{
	}

	rsl::unique_ptr<GameLoop> IntroLoop::run()
	{
		// What's the best API to go through init -> loop -> end?

		m_active_loop->run();

		m_loop_runner.transition_loop(m_display_copyright, m_display_gamefreak_logo);

		switch (m_state)
		{
		case pokemon::IntroLoop::State::Copyright:
			display_copyright();
			break;
		case pokemon::IntroLoop::State::GamefreakLogo:
			display_gamefreak_logo();
			break;
		case pokemon::IntroLoop::State::Fight:
			display_fight();
			break;
		case pokemon::IntroLoop::State::StartMenu:
			display_start_menu();
			break;
		default:
			break;
		}
	}

	void IntroLoop::display_copyright()
	{
		// load the big star and spawn it in the top right of the screen

		// move the big star bottom left

		// when it reached the bottom left, flash the game freak icon

		m_state = State::GamefreakLogo;
	}
	void IntroLoop::display_gamefreak_logo()
	{
		// Check for player input every 10 frames

		// Move the star a few pixels

		m_state = State::Fight;
	}
	void IntroLoop::display_fight()
	{
		m_state = State::StartMenu;
	}
	void IntroLoop::display_start_menu()
	{

	}

}