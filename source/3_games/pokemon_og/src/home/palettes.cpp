// ===============================
// Screen / Palette routines
// C++ translation (continuation)
// ===============================

#include <cstdint>

// extern engine functions
extern void ClearSprites();
extern void ReloadMapSpriteTilePatterns();
extern void LoadScreenTilesFromBuffer2();
extern void LoadTextBoxTilePatterns();
extern void RunDefaultPaletteCommand();
extern void DelayFrames(uint8_t c);
extern void GBPalWhiteOut();
extern void RunPaletteCommand_Predef(uint8_t b);

// hardware registers (mocked)
extern uint8_t rBGP;
extern uint8_t rOBP0;
extern uint8_t rOBP1;

// globals
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t wOnSGB;

// constants
constexpr uint8_t SET_PAL_DEFAULT = 0;

// ===============================
// RestoreScreenTilesAndReloadTilePatterns
// ===============================
void RestoreScreenTilesAndReloadTilePatterns()
{
  ClearSprites();

  wUpdateSpritesEnabled = 1;

  ReloadMapSpriteTilePatterns();
  LoadScreenTilesFromBuffer2();
  LoadTextBoxTilePatterns();

  RunDefaultPaletteCommand();

  Delay3();
}

// ===============================
// Delay3
// ===============================
void Delay3()
{
  DelayFrames(3);
}

// ===============================
// GBPalWhiteOutWithDelay3
// ===============================
void GBPalWhiteOutWithDelay3()
{
  GBPalWhiteOut();
  Delay3();
}

// ===============================
// GBPalNormal
// ===============================
void GBPalNormal()
{
  rBGP = 0b11100100;
  rOBP0 = 0b11010000;
}

// ===============================
// GBPalWhiteOut
// ===============================
void GBPalWhiteOut()
{
  rBGP = 0;
  rOBP0 = 0;
  rOBP1 = 0;
}

// ===============================
// RunDefaultPaletteCommand
// ===============================
void RunDefaultPaletteCommand()
{
  uint8_t b = SET_PAL_DEFAULT;
  RunPaletteCommand_Predef(b);
}

// ===============================
// GetHealthBarColor
// ===============================
void GetHealthBarColor(uint8_t e, uint8_t* hl)
{
  uint8_t color;

  if (e >= 27)
  {
    color = 0; // green
  }
  else if (e >= 10)
  {
    color = 1; // yellow
  }
  else
  {
    color = 2; // red
  }

  *hl = color;
}