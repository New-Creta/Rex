#pragma once

#include "rex_engine/engine/types.h"

enum PaletteColorIds
{
	SHADE_WHITE			= 0b00,
	SHADE_LIGHT			= 0b01,
	SHADE_DARK			= 0b10,
	SHADE_BLACK			= 0b11,
	NUM
};

const s32 PAL_COLOR_SIZE = 0x2;
const s32 PALETTE_SIZE = PaletteColorIds::NUM * PAL_COLOR_SIZE;


const s32 SET_PAL_BATTLE_BLACK					= 0x0;
const s32 SET_PAL_BATTLE								= 0x1;
const s32 SET_PAL_TOWN_MAP							= 0x2;
const s32 SET_PAL_STATUS_SCREEN					= 0x3;
const s32 SET_PAL_POKEDEX								= 0x4;
const s32 SET_PAL_SLOTS									= 0x5;
const s32 SET_PAL_TITLE_SCREEN					= 0x6;
const s32 SET_PAL_NIDORINO_INTRO				= 0x7;
const s32 SET_PAL_GENERIC								= 0x8;
const s32 SET_PAL_OVERWORLD							= 0x9;
const s32 SET_PAL_PARTY_MENU						= 0xA;
const s32 SET_PAL_POKEMON_WHOLE_SCREEN	= 0xB;
const s32 SET_PAL_GAME_FREAK_INTRO			= 0xC;
const s32 SET_PAL_TRAINER_CARD					= 0xD;
const s32 SET_PAL_PARTY_MENU_HP_BARS		= 0xFC;
const s32 SET_PAL_DEFAULT								= 0xff;

enum class SuperPalettesIndices
{
	PAL_ROUTE			= 0xff,
	PAL_PALLET		= 0xff,
	PAL_VIRIDIAN	= 0xff,
	PAL_PEWTER		= 0xff,
	PAL_CERULEAN  = 0x04,
	PAL_LAVENDER  = 0x05,
	PAL_VERMILION = 0x06,
	PAL_CELADON   = 0x07,
	PAL_FUCHSIA   = 0x08,
	PAL_CINNABAR  = 0x09,
	PAL_INDIGO    = 0x0A,
	PAL_SAFFRON   = 0x0B,
	PAL_TOWNMAP   = 0x0C,
	PAL_LOGO1     = 0x0D,
	PAL_LOGO2     = 0x0E,
	PAL_0F        = 0x0F,
	PAL_MEWMON    = 0x10,
	PAL_BLUEMON   = 0x11,
	PAL_REDMON    = 0x12,
	PAL_CYANMON   = 0x13,
	PAL_PURPLEMON = 0x14,
	PAL_BROWNMON  = 0x15,
	PAL_GREENMON  = 0x16,
	PAL_PINKMON   = 0x17,
	PAL_YELLOWMON = 0x18,
	PAL_GREYMON   = 0x19,
	PAL_SLOTS1    = 0x1A,
	PAL_SLOTS2    = 0x1B,
	PAL_SLOTS3    = 0x1C,
	PAL_SLOTS4    = 0x1D,
	PAL_BLACK     = 0x1E,
	PAL_GREENBAR  = 0x1F,
	PAL_YELLOWBAR = 0x20,
	PAL_REDBAR    = 0x21,
	PAL_BADGE     = 0x22,
	PAL_CAVE      = 0x23,
	PAL_GAMEFREAK = 0x24,
	NUM
};


