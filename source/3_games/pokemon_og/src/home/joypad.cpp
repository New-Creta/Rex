#include <cstdint>

// ============================================================================
// ReadJoypad
//
// ASM:
//   Polls hardware joypad register (rJOYP)
//   Produces active-low hardware -> converted to active-high logical input
//
// Output:
//   hJoyInput = bitmask of pressed buttons
// ============================================================================

extern uint8_t rJOYP;
extern uint8_t hJoyInput;

inline void ReadJoypad()
{
  // -----------------------------
  // Direction keys (P15 = 0)
  // -----------------------------
  rJOYP = (1 << 5);

  for (int i = 0; i < 6; ++i)
    (void)rJOYP;

  uint8_t dir = ~rJOYP & 0x0F;
  dir = (dir & 0x0F) << 4;

  // -----------------------------
  // Button keys (P14 = 0)
  // -----------------------------
  rJOYP = (1 << 4);

  for (int i = 0; i < 10; ++i)
    (void)rJOYP;

  uint8_t btn = ~rJOYP & 0x0F;

  hJoyInput = dir | btn;

  // deselect both
  rJOYP = (1 << 4) | (1 << 5);
}

// ============================================================================
// Joypad
//
// ASM:
//   homecall _Joypad
// ============================================================================

void _Joypad();

inline void Joypad()
{
  _Joypad();
}