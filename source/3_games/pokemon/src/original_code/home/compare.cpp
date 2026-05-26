#include <cstdint>

// ============================================================================
// StringCmp
//
// ASM:
//   ; Compare strings, c bytes in length, at de and hl.
//   ; Often used to compare big endian numbers in battle calculations.
//
// Returns:
//   0  -> equal
//   <0 -> de < hl
//   >0 -> de > hl
// ============================================================================

inline int StringCmp(
  const uint8_t* de,
  const uint8_t* hl,
  uint8_t c)
{
  while (c != 0)
  {
    // ASM:
    //   ld a, [de]
    //   cp [hl]
    if (*de != *hl)
    {
      return static_cast<int>(*de)
        - static_cast<int>(*hl);
    }

    // ASM:
    //   inc de
    //   inc hl
    ++de;
    ++hl;

    // ASM:
    //   dec c
    --c;
  }

  return 0;
}