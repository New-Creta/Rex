#include <cstdint>

// ============================================================================
// Game Boy vector table (ROM layout representation in C++)
//
// NOTE:
// This is NOT real C++ execution code — it's a structural translation of ROM
// placement + interrupt behavior.
// ============================================================================

// ---------------------------------------------------------------------------
// RST vectors (unused, all crash intentionally)
// ---------------------------------------------------------------------------

[[noreturn]] inline void rst0() { while (true) {} } // rst $00/$38 behavior placeholder
[[noreturn]] inline void rst8() { while (true) {} }
[[noreturn]] inline void rst10() { while (true) {} }
[[noreturn]] inline void rst18() { while (true) {} }
[[noreturn]] inline void rst20() { while (true) {} }
[[noreturn]] inline void rst28() { while (true) {} }
[[noreturn]] inline void rst30() { while (true) {} }
[[noreturn]] inline void rst38() { while (true) {} }

// ---------------------------------------------------------------------------
// Hardware interrupt handlers (external engine entry points)
// ---------------------------------------------------------------------------

void VBlank();
void Timer();
void Serial();

// Joypad interrupt does nothing (reti)
inline void Joypad() {}

// ---------------------------------------------------------------------------
// Interrupt vector mapping (conceptual)
// ---------------------------------------------------------------------------

using ISR = void(*)();

constexpr ISR VBLANK_VECTOR = VBlank; // $0040
constexpr ISR LCD_VECTOR = nullptr; // rst $38 (unused)
constexpr ISR TIMER_VECTOR = Timer;   // $0050
constexpr ISR SERIAL_VECTOR = Serial;  // $0058
constexpr ISR JOYPAD_VECTOR = Joypad;  // $0060

// ---------------------------------------------------------------------------
// ROM entry point ($0100)
// ---------------------------------------------------------------------------

extern void _Start();

extern "C" void GameBoyEntry()
{
  // nop
  // jp _Start
  _Start();
}

// ---------------------------------------------------------------------------
// Padding / unused ROM space
// ---------------------------------------------------------------------------

static_assert(true, "ROM padding regions ($0150 onward) are handled by build system");