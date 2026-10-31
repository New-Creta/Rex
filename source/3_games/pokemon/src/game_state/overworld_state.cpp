#include "pokemon/game_state/overworld_state.h"

namespace pokemon
{
	OverworldState::OverworldState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void OverworldState::begin()
	{

	}
	GameState OverworldState::tick(s32 frameIdx)
	{
		return GameState::Overworld;
	}
	void OverworldState::end()
	{

	}
}