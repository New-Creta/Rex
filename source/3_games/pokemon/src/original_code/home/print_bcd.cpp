#include <cstdint>
#include <cstring>

using u8 = uint8_t;

// ------------------------------------------------------------
// External globals / engine state
// ------------------------------------------------------------
u8 wCurrencySymbolEnabled = 0;

void PrintLetterDelay();

// ------------------------------------------------------------
// PrintBCDNumber
// ------------------------------------------------------------

void PrintBCDDigit(u8 digit, u8& flags, char*& hl);

void PrintBCDNumber(u8* de, char*& hl, u8 c)
{
  u8 b = c;

  c &= 0x1F;        // length only (bits 0-4)
  bool noLeadingZero = b & 0x80;
  bool leftAlign = b & 0x40;
  bool printCurrency = b & 0x20;

  bool printedAnyNonZero = false;

  // Currency symbol at start (if enabled and not suppressed by no-leading-zero behavior)
  if (printCurrency && !noLeadingZero)
  {
    *hl++ = '¥';
  }

  for (u8 i = 0; i < c; i++)
  {
    u8 byte = *de++;

    u8 high = (byte >> 4) & 0xF;
    u8 low = byte & 0xF;

    PrintBCDDigit(high, b, hl);
    PrintBCDDigit(low, b, hl);
  }

  if (!(b & 0x80))
    return;

  // all digits were zero
  if (!leftAlign)
    --hl;

  if (printCurrency)
    *hl++ = '¥';

  *hl++ = '0';
  PrintLetterDelay();
}

// ------------------------------------------------------------
// PrintBCDDigit
// ------------------------------------------------------------

void PrintBCDDigit(u8 digit, u8& b, char*& hl)
{
  digit &= 0xF;

  if (digit == 0)
  {
    if (b & 0x80)
    {
      if (b & 0x40)
        return;

      *hl++ = ' ';
      return;
    }

    goto OUTPUT;
  }

  if (b & 0x80)
  {
    if (b & 0x20)
    {
      *hl++ = '¥';
      b &= ~0x20;
    }

    b &= ~0x80;
  }

OUTPUT:
  *hl++ = char('0' + digit);
  PrintLetterDelay();
}