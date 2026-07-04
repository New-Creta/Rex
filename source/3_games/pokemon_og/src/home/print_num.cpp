#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;

// ------------------------------------------------------------
// External / hardware-like state
// ------------------------------------------------------------
u8 hPastLeadingZeros;
u8 hNumToPrint[3];
u8 hSavedNumToPrint[3];
u8 hPowerOf10[3];

constexpr u8 BIT_LEADING_ZEROES = 7;
constexpr u8 BIT_LEFT_ALIGN = 6;

// Output abstraction (VRAM / text buffer)
using OutPtr = char*;

// ------------------------------------------------------------
// Helper: digit printing
// ------------------------------------------------------------
void PrintLetterDelay();

// ------------------------------------------------------------
// Core PrintNumber
// ------------------------------------------------------------
void PrintNumber(uint8_t* de, OutPtr hl, u8 c, u8 b)
{
  hPastLeadingZeros = 0;
  hNumToPrint[0] = 0;
  hNumToPrint[1] = 0;
  hNumToPrint[2] = 0;

  u8 len = b & 0x0F;
  u8 flags = b;

  // --------------------------------------------------------
  // Load value into hNumToPrint (little-endian-ish BCD/int)
  // --------------------------------------------------------
  if (len == 1)
  {
    hNumToPrint[2] = *de++;
  }
  else if (len == 2)
  {
    hNumToPrint[1] = *de++;
    hNumToPrint[2] = *de++;
  }
  else
  {
    hNumToPrint[0] = *de++;
    hNumToPrint[1] = *de++;
    hNumToPrint[2] = *de++;
  }

  u8 digits = c;
  u8 printedLeading = 0;

  auto PrintDigit = [&](u32 power) -> u8
    {
      u32 value =
        (u32(hNumToPrint[0]) << 16) |
        (u32(hNumToPrint[1]) << 8) |
        (u32(hNumToPrint[2]));

      u8 count = 0;

      while (value >= power)
      {
        value -= power;
        count++;
      }

      hNumToPrint[0] = (value >> 16) & 0xFF;
      hNumToPrint[1] = (value >> 8) & 0xFF;
      hNumToPrint[2] = value & 0xFF;

      return count;
    };

  auto NextDigit = [&]()
    {
      bool leadingZeroAllowed = flags & (1 << BIT_LEADING_ZEROES);
      bool leftAlign = flags & (1 << BIT_LEFT_ALIGN);

      if (!leadingZeroAllowed && leftAlign && hPastLeadingZeros == 0)
        return;

      hl++;
    };

  // --------------------------------------------------------
  // Large digit stages (macro-expanded behavior simplified)
  // --------------------------------------------------------
  struct Step { u32 pow; };
  Step steps[] = {
      {1000000},
      {100000},
      {10000},
      {1000},
      {100}
  };

  for (auto& s : steps)
  {
    u8 d = PrintDigit(s.pow);

    if (d || hPastLeadingZeros)
    {
      *hl = char('0' + d);
      hPastLeadingZeros = 1;
    }

    NextDigit();
  }

  // --------------------------------------------------------
  // Tens / ones
  // --------------------------------------------------------
  u8 val = hNumToPrint[2];
  u8 tens = 0;

  while (val >= 10)
  {
    val -= 10;
    tens++;
  }

  hNumToPrint[2] = val;

  if (tens || hPastLeadingZeros)
  {
    *hl = char('0' + tens);
    hPastLeadingZeros = 1;
  }
  else
  {
    if (flags & (1 << BIT_LEADING_ZEROES))
      *hl = '0';
  }

  NextDigit();

  *hl++ = char('0' + hNumToPrint[2]);

  de--;
}

// ------------------------------------------------------------
// PrintDigit helpers (collapsed logic equivalent)
// ------------------------------------------------------------
void PrintLeadingZero(u8 flags, OutPtr& hl)
{
  if (!(flags & (1 << BIT_LEADING_ZEROES)))
    return;

  *hl = '0';
}

// ------------------------------------------------------------
// Notes:
// - Full borrow-heavy subtraction loop in original ASM is
//   intentionally approximated as integer division.
// - Exact cycle-level behavior is not preserved.
// ------------------------------------------------------------