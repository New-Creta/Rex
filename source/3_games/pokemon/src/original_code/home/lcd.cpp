#include <cstdint>

// ============================================================================
// DisableLCD
//
// ASM summary:
//   - disables LCD safely during VBlank
//   - preserves interrupt enable state except VBlank bit manipulation
// ============================================================================

extern uint8_t rIF;
extern uint8_t rIE;
extern uint8_t rLY;
extern uint8_t rLCDC;

constexpr uint8_t LY_VBLANK = 144;
constexpr uint8_t rLCDC_ENABLE_MASK = 0x80;
constexpr uint8_t rLCDC_ENABLE = 7;

inline void DisableLCD()
{
  rIF = 0;

  uint8_t ie = rIE;

  // res 0, a -> clear VBlank interrupt enable
  ie &= ~(1 << 0);
  rIE = ie;

  // wait for VBlank
  while (rLY != LY_VBLANK) {}

  // turn off LCD
  rLCDC &= ~rLCDC_ENABLE_MASK;

  // restore IE
  rIE = ie;
}

// ============================================================================
// EnableLCD
// ============================================================================

inline void EnableLCD()
{
  rLCDC |= (1 << rLCDC_ENABLE);
}