// =========================================================
// Random
// =========================================================

// mock RAM byte used by original engine
uint8_t hRandomAdd;

// external RNG routine (farcall Random_)
void Random_();

void Random()
{
  // push hl/de/bc (no-op in C++ scope)

  Random_();

  // ld a, [hRandomAdd]
  uint8_t a = hRandomAdd;

  // pop bc/de/hl (no-op)

  // original ASM returns A in register; here we just return via function return
  (void)a;
}