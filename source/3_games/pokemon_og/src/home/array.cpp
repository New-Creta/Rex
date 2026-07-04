#include <cstdint>

// Equivalent to:
// SkipFixedLengthTextEntries::
//     hl += NAME_LENGTH * a
//
// Parameters:
// - hl: pointer/reference to current location
// - a : number of entries to skip
//
inline void SkipFixedLengthTextEntries(uint8_t*& hl, uint8_t a)
{
  if (a == 0)
    return;

  hl += NAME_LENGTH * a;
}

// Equivalent to:
// AddNTimes::
//     hl += bc * a
//
// Parameters:
// - hl: pointer/reference being advanced
// - bc: stride amount
// - a : number of times
//
inline void AddNTimes(uint8_t*& hl, uint16_t bc, uint8_t a)
{
  if (a == 0)
    return;

  hl += bc * a;
}