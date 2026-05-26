#include "pokemon/original_code/home/init.h"
#include "pokemon/original_code/gameboy_instructions.h"
#include "pokemon/original_code/constants/hardware_constants.h"
#include "pokemon/original_code/home/lcd.h"
#include "pokemon/original_code/home/clear_sprites.h"
#include "pokemon/original_code/ram/hram.h"
#include "pokemon/original_code/ram/wram.h"
#include "pokemon/original_code/constants/serial_constants.h"

#include "rex_engine/engine/types.h"

namespace pokemon
{
	void StopAllSounds()
	{
		s8 audio_engine_1_sound_bank = 0;
		wram.audio_data.wAudioROMBank = audio_engine_1_sound_bank;
		wram.audio_data.wAudioSavedROMBank = audio_engine_1_sound_bank;

		wram.data.wAudioFadeOutControl = 0;
		wram.audio_data.wNewSoundID = 0;
		wram.data.wLastMusicSoundID = 0;

		PlaySound();
	}

	void init()
	{
		const s8 rLCDC_DEFAULT = 0b11100011;

		// *LCD enabled
		// *Window tile map at $9C00
		// *Window display enabled
		// *BG and window tile data at $8800
		// *BG tile map at $9800
		// *8x8 OBJ size
		// *OBJ display enabled
		// *BG display enabled
		gameboy_di();

		*rIF = 0;
		*rIE = 0;
		*rSCX = 0;
		*rSCY = 0;
		*rSB = 0;
		*rSC = 0;
		*rWX = 0;
		*rWY = 0;
		*rTMA = 0;
		*rTAC = 0;
		*rBGP = 0;
		*rOBP0 = 0;
		*rOBP1 = 0;
		*rLCDC = rLCDC_ENABLE_MASK;

		DisableLCD();

		// The original code clears the stack, VRAM and WRAM here. 
		// However, as we run on a machine with its own OS, we don't need to do that

		ClearSprites();

		// Some memory bank loading gets done at this point in the original code
		
		hram.hTileAnimations = 0;
		rSTAT = 0;
		hram.hSCX = 0;
		hram.hSCY = 0;
		*rIF = 0;
		*rIE = 1 << VBLANK + 1 << TIMER + 1 << SERIAL;

		hram.hWY = 144;
		*rWY = 144;
		*rWX = 7;

		hram.hSerialConnectionStatus = CONNECTION_NOT_ESTABLISHED;

		// Original code clears the VRAM back buffer of the map,
		// which is not really possible here of course
		
		*rLCDC = rLCDC_DEFAULT;
		hram.hSoftReset = 16;

		StopAllSounds();
	}
}



#include <cstdint>

// ============================================================================
// SoftReset / Init
// ============================================================================

extern uint8_t rIF, rIE, rSCX, rSCY, rSB, rSC, rWX, rWY, rTMA, rTAC;
extern uint8_t rBGP, rOBP0, rOBP1, rLCDC, rSTAT, rWY, rWX;

extern uint8_t hLoadedROMBank;
extern uint8_t hTileAnimations;
extern uint8_t hSCX, hSCY;
extern uint8_t hWY;
extern uint8_t hSoftReset;
extern uint8_t hSerialConnectionStatus;

extern uint8_t hAutoBGTransferDest[];

extern uint8_t wStack;

extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wAudioFadeOutControl;
extern uint8_t wNewSoundID;
extern uint8_t wLastMusicSoundID;
extern uint8_t wUpdateSpritesEnabled;

constexpr uint8_t CONNECTION_NOT_ESTABLISHED = 0;
constexpr uint8_t rLCDC_ENABLE_MASK = 0x91; // placeholder

constexpr uint8_t VBLANK = 0;
constexpr uint8_t TIMER = 0;
constexpr uint8_t SERIAL = 0;

// External engine calls
void StopAllSounds();
void GBPalWhiteOut();
void GBPalNormal();
void DisableLCD();
void ClearVram();
void ClearSprites();
void WriteDMACodeToHRAM();
void ClearBgMap(uint8_t h);
void FillMemory(uint8_t* hl, uint16_t bc);
void DelayFrames(uint8_t c);
void PrepareTitleScreen();

// predef calls (engine entry points)
void LoadSGB();
void PlayIntro();

// ============================================================================
// SoftReset
// ============================================================================

inline void SoftReset()
{
  StopAllSounds();
  GBPalWhiteOut();
  DelayFrames(32);

  // fallthrough to Init
}

// ============================================================================
// Init
// ============================================================================

inline void Init()
{
  // di -> interrupts disabled (implicit in C++ context)

  rIF = 0;
  rIE = 0;
  rSCX = 0;
  rSCY = 0;
  rSB = 0;
  rSC = 0;
  rWX = 0;
  rWY = 0;
  rTMA = 0;
  rTAC = 0;
  rBGP = 0;
  rOBP0 = 0;
  rOBP1 = 0;

  rLCDC = rLCDC_ENABLE_MASK;
  DisableLCD();

  // sp = wStack (not representable directly)

  // clear WRAM
  {
    uint8_t* hl = reinterpret_cast<uint8_t*>(0); // STARTOF(WRAM0)
    uint16_t bc = 0; // SIZEOF(WRAM0)

    while (bc--) *hl++ = 0;
  }

  ClearVram();

  // clear HRAM
  {
    uint8_t* hl = reinterpret_cast<uint8_t*>(0); // HRAM
    uint16_t bc = 0;
    FillMemory(hl, bc);
  }

  ClearSprites();

  // DMA code upload
  hLoadedROMBank = 0; // BANK(WriteDMACodeToHRAM)
  WriteDMACodeToHRAM();

  hTileAnimations = 0;
  rSTAT = 0;
  hSCX = 0;
  hSCY = 0;
  rIF = 0;

  rIE = (1 << VBLANK) | (1 << TIMER) | (1 << SERIAL);

  hWY = 144;
  rWY = 144;

  rWX = 7;

  hSerialConnectionStatus = CONNECTION_NOT_ESTABLISHED;

  ClearBgMap(0);
  ClearBgMap(1);

  rLCDC = rLCDC_ENABLE_MASK;

  hSoftReset = 16;

  StopAllSounds();

  // ei -> interrupts enabled (implicit)

  LoadSGB();

  wAudioROMBank = 0;
  wAudioSavedROMBank = 0;

  hAutoBGTransferDest[1] = 0x9C;
  hAutoBGTransferDest[0] = 0;

  wUpdateSpritesEnabled = 0xFF;

  PlayIntro();

  DisableLCD();
  ClearVram();
  GBPalNormal();
  ClearSprites();

  rLCDC = rLCDC_ENABLE_MASK;

  PrepareTitleScreen();
}

// ============================================================================
// ClearVram
// ============================================================================

inline void ClearVram()
{
  uint8_t* hl = reinterpret_cast<uint8_t*>(0); // VRAM start
  uint16_t bc = 0; // SIZEOF(VRAM)

  while (bc--)
    *hl++ = 0;
}

// ============================================================================
// StopAllSounds
// ============================================================================

inline void StopAllSounds()
{
  wAudioROMBank = 0;
  wAudioSavedROMBank = 0;

  wAudioFadeOutControl = 0;
  wNewSoundID = 0;
  wLastMusicSoundID = 0;

  wNewSoundID = 0xFF;

  PlaySound(wNewSoundID);
}