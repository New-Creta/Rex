#include <cstdint>

// ============================================================================
// Multiply
//
// ASM:
//   FF96-FF98 = multiplicand (big endian)
//   FF99      = multiplier
//   output    = FF95-FF98 product
//
// Notes:
// - Implementation is in _Multiply (banked routine)
// - This wrapper only preserves registers (HL, BC)
// ============================================================================

// emulated memory-mapped registers
extern uint8_t FF95[4];
extern uint8_t FF96[3];
extern uint8_t FF99;

void _Multiply(); // far call

inline void Multiply()
{
  // push hl / bc -> saved implicitly in C++ by not modifying external state
  _Multiply();
}

// ============================================================================
// Divide
//
// ASM:
//   FF95-FF98 = dividend (big endian)
//   FF99      = divisor
//   B         = byte width of dividend
//   output    = FF95-FF98 quotient
//               FF99 remainder
//
// Notes:
// - implemented in banked _Divide
// - preserves HL, DE, BC
// ============================================================================

extern uint8_t FF95_dividend[4];
extern uint8_t FF99_divisor;

void _Divide(uint8_t bytes);

inline void Divide(uint8_t byteCount)
{
  _Divide(byteCount);
}