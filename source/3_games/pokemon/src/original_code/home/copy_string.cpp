#include <cstdint>

// ============================================================================
// CopyToStringBuffer / CopyString
//
// ASM:
//   CopyToStringBuffer:
//     hl = wStringBuffer
//     fallthrough
//
//   CopyString:
//     while (true):
//       a = [de]
//       [hl++] = a
//       if a == "@": break
//
// Notes:
//   - Strings are '@' terminated (0x40 in Gen1/Gen2 Pokémon-style text)
// ============================================================================

extern uint8_t wStringBuffer[];

// Copies from DE -> HL until '@' terminator
inline void CopyString(uint8_t* hl, const uint8_t* de)
{
  while (true)
  {
    uint8_t a = *de++;
    *hl++ = a;

    if (a == '@')
      break;
  }
}

// Wrapper that targets wStringBuffer
inline void CopyToStringBuffer(const uint8_t* de)
{
  CopyString(wStringBuffer, de);
}