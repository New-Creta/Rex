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
	GameLoopState StartMenuState::tick(s32 frameIdx)
	{

	}
	void StartMenuState::end()
	{

	}
}