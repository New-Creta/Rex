#include "pokemon/game_state/oak_intro_state.h"

namespace pokemon
{
	OakIntroState::OakIntroState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void OakIntroState::begin()
	{

	}
	GameLoopState OakIntroState::tick(s32 frameIdx)
	{

	}
	void OakIntroState::end()
	{

	}
}