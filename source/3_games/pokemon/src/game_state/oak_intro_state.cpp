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
	GameState OakIntroState::tick(s32 frameIdx)
	{
		return GameState::OakIntro;
	}
	void OakIntroState::end()
	{

	}
}