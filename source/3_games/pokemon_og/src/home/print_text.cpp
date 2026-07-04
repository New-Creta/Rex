// === C++ translation of selected overworld.asm routines ===
// (partial continuation)

#include <cstdint>
#include <cstring>

// --- mock hardware / memory ---
uint8_t wd730;
uint8_t wLetterPrintingDelayFlags;
uint8_t wOptions;
uint8_t hFrameCounter;
uint8_t hJoyHeld;

constexpr uint8_t BIT_A_BUTTON = 0;
constexpr uint8_t BIT_B_BUTTON = 1;

// --- external engine stubs ---
void Joypad() {}
void DelayFrame() {}

// =========================================================
// PrintLetterDelay
// =========================================================
void PrintLetterDelay()
{
  if (wd730 & (1 << 6))
    return;

  if (!(wLetterPrintingDelayFlags & (1 << 1)))
    return;

  // registers preserved in ASM (no-op in C++ scope)

  uint8_t flags = wLetterPrintingDelayFlags;

  bool useOptionDelay = flags & 0x01;

  if (!useOptionDelay)
  {
    hFrameCounter = 1;
  }
  else
  {
    hFrameCounter = wOptions & 0x0F;
  }

  while (true)
  {
    Joypad();
    uint8_t joy = hJoyHeld;

    if (joy & (1 << BIT_A_BUTTON))
      break;

    if (joy & (1 << BIT_B_BUTTON))
      break;

    DelayFrame();

    if (hFrameCounter != 0)
    {
      hFrameCounter--;
      if (hFrameCounter == 0)
        break;
    }
  }
}