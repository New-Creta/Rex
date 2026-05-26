#include <cstdint>

extern uint8_t wShadowOAM[];
extern uint8_t wShadowOAMEnd[];

// ============================================================================
// ClearSprites
//
// ASM:
//   - Clears entire shadow OAM buffer to 0
// ============================================================================

inline void ClearSprites()
{
  constexpr size_t size =
    wShadowOAMEnd - wShadowOAM;

  for (size_t i = 0; i < size; ++i)
  {
    wShadowOAM[i] = 0;
  }
}

// ============================================================================
// HideSprites
//
// ASM:
//   - Sets Y coordinate of all 40 sprites to 160
//   - Each OAM entry is 4 bytes
//   - Writing Y=160 hides sprite on Game Boy hardware
// ============================================================================

inline void HideSprites()
{
  constexpr uint8_t HIDDEN_Y = 160;
  constexpr uint8_t SPRITE_COUNT = 40;
  constexpr uint8_t OAM_ENTRY_SIZE = 4;

  for (uint8_t i = 0; i < SPRITE_COUNT; ++i)
  {
    // First byte of each OAM entry = Y position
    wShadowOAM[i * OAM_ENTRY_SIZE] = HIDDEN_Y;
  }
}