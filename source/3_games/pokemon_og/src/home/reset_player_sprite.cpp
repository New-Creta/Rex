// =========================================================
// ResetPlayerSpriteData
// =========================================================

#include <cstdint>
#include <cstring>

// mock sprite state structures (flat memory model)
uint8_t wSpriteStateData1[0x100];
uint8_t wSpriteStateData2[0x100];

uint8_t wSpritePlayerStateData1PictureID;
uint8_t wSpritePlayerStateData2ImageBaseOffset;
uint8_t wSpritePlayerStateData1YPixels;
uint8_t wSpritePlayerStateData1XPixels;

// external
void FillMemory(uint8_t* hl, uint16_t bc, uint8_t value);

// =========================================================
// ResetPlayerSpriteData_ClearSpriteData
// =========================================================
void ResetPlayerSpriteData_ClearSpriteData(uint8_t* hl)
{
  uint16_t bc = 0x10;
  uint8_t a = 0;

  FillMemory(hl, bc, a);
}

// =========================================================
// ResetPlayerSpriteData
// =========================================================
void ResetPlayerSpriteData()
{
  ResetPlayerSpriteData_ClearSpriteData(wSpriteStateData1);
  ResetPlayerSpriteData_ClearSpriteData(wSpriteStateData2);

  uint8_t a = 1;

  wSpritePlayerStateData1PictureID = a;
  wSpritePlayerStateData2ImageBaseOffset = a;

  uint8_t* hl = &wSpritePlayerStateData1YPixels;

  *hl = 0x3C;          // Y screen pos
  *(hl + 2) = 0x40;    // X screen pos (inc hl twice in ASM)
}