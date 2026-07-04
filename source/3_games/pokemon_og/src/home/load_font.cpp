#include <cstdint>

// ============================================================================
// Notes:
// - These routines switch between "instant VRAM upload" vs "VBlank-safe DMA"
// - In C++ we model this as two different upload paths:
//   * immediateCopy(...) when LCD is off
//   * vblankCopy(...) when LCD is on
// ============================================================================

extern uint8_t rLCDC;

constexpr uint8_t LCD_ENABLE_BIT = 7;

// ROM data (external assets)
extern const uint8_t FontGraphics[];
extern const uint8_t FontGraphicsEnd[];
extern const uint8_t TextBoxGraphics[];
extern const uint8_t TextBoxGraphicsEnd[];
extern const uint8_t HpBarAndStatusGraphics[];
extern const uint8_t HpBarAndStatusGraphicsEnd[];

// VRAM targets (abstracted pointers here)
extern uint8_t vFont[];
extern uint8_t vChars2[];

// Engine primitives (from earlier conversion context)
void FarCopyDataDouble(uint8_t bank, const uint8_t* src, uint8_t* dst, uint16_t size);
void CopyVideoDataDouble(uint8_t bank, const uint8_t* src, uint8_t* dst, uint16_t tiles);
void FarCopyData2(uint8_t bank, const uint8_t* src, uint8_t* dst, uint16_t size);
void CopyVideoData(uint8_t bank, const uint8_t* src, uint8_t* dst, uint16_t tiles);

// ============================================================================
// LoadFontTilePatterns
// ============================================================================
inline void LoadFontTilePatterns()
{
  bool lcdOn = (rLCDC & (1 << LCD_ENABLE_BIT));

  if (!lcdOn)
  {
    // LCD off -> safe bulk transfer
    FarCopyDataDouble(
      /*BANK(FontGraphics)*/ 0,
      FontGraphics,
      vFont,
      (uint16_t)(FontGraphicsEnd - FontGraphics)
    );
  }
  else
  {
    // LCD on -> VBlank chunked transfer
    CopyVideoDataDouble(
      /*BANK(FontGraphics)*/ 0,
      FontGraphics,
      vFont,
      (uint16_t)((FontGraphicsEnd - FontGraphics) / 8)
    );
  }
}

// ============================================================================
// LoadTextBoxTilePatterns
// ============================================================================
inline void LoadTextBoxTilePatterns()
{
  bool lcdOn = (rLCDC & (1 << LCD_ENABLE_BIT));

  uint8_t* dst = vChars2 + (0x60 * 16);

  if (!lcdOn)
  {
    FarCopyData2(
      /*BANK(TextBoxGraphics)*/ 0,
      TextBoxGraphics,
      dst,
      (uint16_t)(TextBoxGraphicsEnd - TextBoxGraphics)
    );
  }
  else
  {
    CopyVideoData(
      /*BANK(TextBoxGraphics)*/ 0,
      TextBoxGraphics,
      dst,
      (uint16_t)((TextBoxGraphicsEnd - TextBoxGraphics) / 16)
    );
  }
}

// ============================================================================
// LoadHpBarAndStatusTilePatterns
// ============================================================================
inline void LoadHpBarAndStatusTilePatterns()
{
  bool lcdOn = (rLCDC & (1 << LCD_ENABLE_BIT));

  uint8_t* dst = vChars2 + (0x62 * 16);

  if (!lcdOn)
  {
    FarCopyData2(
      /*BANK(HpBarAndStatusGraphics)*/ 0,
      HpBarAndStatusGraphics,
      dst,
      (uint16_t)(HpBarAndStatusGraphicsEnd - HpBarAndStatusGraphics)
    );
  }
  else
  {
    CopyVideoData(
      /*BANK(HpBarAndStatusGraphics)*/ 0,
      HpBarAndStatusGraphics,
      dst,
      (uint16_t)((HpBarAndStatusGraphicsEnd - HpBarAndStatusGraphics) / 16)
    );
  }
}