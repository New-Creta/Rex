#include "pokemon/game_loops/intro_loop.h"

namespace pokemon
{
	IntroLoop::IntroLoop()
		: m_state(State::Copyright)
		, m_display_copyright()
		, m_active_loop(&m_display_copyright)
	{
		m_state_tree[State::Copyright] = StateTask(
			[this]() { begin_copyright(); },
			[this]() { tick_copyright(); },
			[this]() { end_copyright(); }
		);
		m_state_tree[State::GamefreakLogo] = StateTask(
			[this]() { begin_gamefreak(); },
			[this]() { tick_gamefreak(); }, 
			[this]() { end_gamefreak(); }
		);
		m_state_tree[State::Fight] = StateTask(
			[this]() { begin_fight_scene(); },
			[this]() { tick_fight_scene(); },
			[this]() { end_fight_scene(); }
		);
		m_state_tree[State::StartMenu] = StateTask(
			[this]() { begin_start_menu(); },
			[this]() { tick_start_menu(); },
			[this]() { end_start_menu(); }
		);
	}

	rsl::unique_ptr<GameLoop> IntroLoop::run()
	{
		// What's the best API to go through init -> loop -> end?
		m_state_tree[m_state].tick();

		m_active_loop->run();

		m_loop_runner.transition_loop(m_display_copyright, display_gamefreak_logo);

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

	// Copyright
	void IntroLoop::begin_copyright()
	{

	}
	void IntroLoop::tick_copyright()
	{

	}
	void IntroLoop::end_copyright()
	{

	}

	// Gamefreak logo
	void IntroLoop::begin_gamefreak()
	{
		
	}
	void IntroLoop::tick_gamefreak()
	{

	}
	void IntroLoop::end_gamefreak()
	{

	}

	// Fight Scene
	void IntroLoop::begin_fight_scene()
	{

	}
	void IntroLoop::tick_fight_scene()
	{

	}
	void IntroLoop::end_fight_scene()
	{

	}

	// Start Menu
	void IntroLoop::begin_start_menu()
	{

	}
	void IntroLoop::tick_start_menu()
	{

	}
	void IntroLoop::end_start_menu()
	{

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