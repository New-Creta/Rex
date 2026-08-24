#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	enum class GameLoopState : s8
	{
		Copyright,
		BlackBorders,
		GamefreakLogo,
		Fight,
		StartMenu,
		OakIntro,
		Overworld,
		Battle
	};
}