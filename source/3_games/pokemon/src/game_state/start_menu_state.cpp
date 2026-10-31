#include "pokemon/game_state/start_menu_state.h"

namespace pokemon
{
	StartMenuState::StartMenuState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void StartMenuState::begin()
	{

	}
	GameState StartMenuState::tick(s32 frameIdx)
	{
		return GameState::StartMenu;
	}
	void StartMenuState::end()
	{

	}
}