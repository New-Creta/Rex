#include <cstdint>

// ============================================================================
// CountSetBits
//
// ASM:
//   hl = address of byte string
//   b  = length
//   output: wNumSetBits = total number of set bits
//
// Notes:
//   - Counts bits using shift + carry accumulation
//   - c accumulates total popcount
// ============================================================================

extern uint8_t wNumSetBits;

inline void CountSetBits(const uint8_t* hl, uint8_t b)
{
  uint8_t c = 0;

  while (b--)
  {
    uint8_t e = *hl++;

    // innerLoop: count bits in byte
    for (uint8_t d = 0; d < 8; ++d)
    {
      // srl e
      uint8_t bit = e & 1;
      e >>= 1;

      // adc c (add carry into accumulator)
      c += bit;
    }
  }

  wNumSetBits = c;
}