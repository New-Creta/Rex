#include <cstdint>

using FuncPtr = void(*)();

// ============================================================================
// CallFunctionInTable
//
// ASM:
//   ; Call function a in jumptable hl.
//   ; de is not preserved.
//
// Equivalent:
//   hl[a]()
//
// Notes:
//   - Jump table entries are 16-bit addresses
//   - "add a" in ASM multiplies index by 2
// ============================================================================

inline void CallFunctionInTable(const FuncPtr* hl, uint8_t a)
{
  hl[a]();
}

// ============================================================================
// IsInArray / IsInRestOfArray
//
// ASM:
//   ; Search an array at hl for the value in a.
//   ; Entry size is de bytes.
//   ; Return count b and carry if found.
//
// Equivalent:
//   - Search first byte of each entry
//   - Entries are DE bytes apart
//   - Array ends at 0xFF sentinel
//   - Returns found flag + index
// ============================================================================

struct ArraySearchResult
{
  bool found;
  uint8_t index;
};

inline ArraySearchResult IsInArray(
  const uint8_t* hl,
  uint8_t value,
  uint16_t de)
{
  uint8_t b = 0;

  while (true)
  {
    uint8_t current = *hl;

    // cp -1
    if (current == 0xFF)
    {
      return { false, b };
    }

    // cp c
    if (current == value)
    {
      return { true, b };
    }

    ++b;

    // add hl, de
    hl += de;
  }
}