#include <cstdint>

// ============================================================================
// FadeOutAudio
//
// ASM:
//   Handles gradual volume fade via NR50 register
//   Uses wAudioFadeOutControl as active fade flag
// ============================================================================

extern uint8_t wAudioFadeOutControl;
extern uint8_t wAudioFadeOutCounter;
extern uint8_t wAudioFadeOutCounterReloadValue;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wAudioROMBank;
extern uint8_t wNewSoundID;
extern uint8_t wd72c;

extern uint8_t rNR50;

constexpr uint8_t SFX_STOP_ALL_MUSIC = 0;

// External
void PlaySound(uint8_t id);

// ============================================================================

inline void FadeOutAudio()
{
  // ld a, [wAudioFadeOutControl]
  if (wAudioFadeOutControl == 0)
  {
    // ld a, [wd72c] / bit 1
    if (wd72c & (1 << 1))
      return;

    // ld a, $77 / ldh [rNR50]
    rNR50 = 0x77;
    return;
  }

  // .fadingOut
  if (wAudioFadeOutCounter != 0)
  {
    --wAudioFadeOutCounter;
    return;
  }

  wAudioFadeOutCounter = wAudioFadeOutCounterReloadValue;

  uint8_t vol = rNR50;

  // if volume already zero -> complete fade
  if (vol == 0)
  {
    uint8_t savedControl = wAudioFadeOutControl;

    wAudioFadeOutControl = 0;

    wNewSoundID = SFX_STOP_ALL_MUSIC;
    PlaySound(wNewSoundID);

    wAudioROMBank = wAudioSavedROMBank;

    wNewSoundID = savedControl;
    PlaySound(wNewSoundID);

    return;
  }

  // lower volume (left + right nibbles)
  uint8_t low = (vol & 0x0F);
  uint8_t high = (vol & 0xF0) >> 4;

  if (low > 0)  --low;
  if (high > 0) --high;

  rNR50 = (high << 4) | low;
}