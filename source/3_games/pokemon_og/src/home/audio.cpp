#include <cstdint>

// ============================================================================
// External engine state / hardware globals
// ============================================================================

extern uint8_t wLastMusicSoundID;
extern uint8_t wWalkBikeSurfState;
extern uint8_t wMapMusicSoundID;
extern uint8_t wMapMusicROMBank;
extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wAudioFadeOutControl;
extern uint8_t wAudioFadeOutCounter;
extern uint8_t wAudioFadeOutCounterReloadValue;
extern uint8_t wNewSoundID;
extern uint8_t wd72e;

extern uint8_t hLoadedROMBank;
extern uint8_t hSavedROMBank;

extern uint8_t MBC1RomBank;

extern uint8_t wChannelSoundIDs[];

constexpr int CHAN5 = 4;
constexpr int CHAN6 = 5;
constexpr int CHAN7 = 6;
constexpr int CHAN8 = 7;

constexpr uint8_t MUSIC_BIKE_RIDING = 0;
constexpr uint8_t MUSIC_SURFING = 0;

constexpr uint8_t AUDIO1_BANK = 0;
constexpr uint8_t AUDIO2_BANK = 0;
constexpr uint8_t AUDIO3_BANK = 0;

// ============================================================================
// External routines
// ============================================================================

void WaitForSoundToFinish();

void Audio1_PlaySound(uint8_t sound);
void Audio2_PlaySound(uint8_t sound);
void Audio3_PlaySound(uint8_t sound);

void Audio1_UpdateMusic();
void Audio2_UpdateMusic();
void Audio3_UpdateMusic();

void Bankswitch();

// ============================================================================
// CompareMapMusicBankWithCurrentBank
//
// Returns:
//   true  = banks differ
//   false = same bank
// ============================================================================

inline bool CompareMapMusicBankWithCurrentBank(uint8_t fadeOutCounter)
{
  uint8_t mapBank = wMapMusicROMBank;

  if (wAudioROMBank == mapBank)
  {
    wAudioSavedROMBank = wAudioROMBank;
    return false;
  }

  // If fade-out isn't active,
  // immediately switch audio bank.
  if (fadeOutCounter == 0)
  {
    wAudioROMBank = mapBank;
  }

  wAudioSavedROMBank = mapBank;

  return true;
}

// ============================================================================
// PlaySound
//
// If sound == 0xFF, music is stopped.
// ============================================================================

inline void PlaySound(uint8_t sound)
{
  uint8_t b = sound;

  // If a new sound is pending,
  // clear channel sound IDs.
  if (wNewSoundID != 0)
  {
    wChannelSoundIDs[CHAN5] = 0;
    wChannelSoundIDs[CHAN6] = 0;
    wChannelSoundIDs[CHAN7] = 0;
    wChannelSoundIDs[CHAN8] = 0;
  }

  // Fade-out requested?
  if (wAudioFadeOutControl != 0)
  {
    // Ignore if new sound ID is zero.
    if (wNewSoundID == 0)
    {
      return;
    }

    wNewSoundID = 0;

    // If music already stopped,
    // bypass fade logic.
    if (wLastMusicSoundID != 0xFF)
    {
      wLastMusicSoundID = b;

      wAudioFadeOutCounterReloadValue =
        wAudioFadeOutControl;

      wAudioFadeOutCounter =
        wAudioFadeOutControl;

      wAudioFadeOutControl = b;

      return;
    }

    wAudioFadeOutControl = 0;
  }

  wNewSoundID = 0;

  // Save current ROM bank.
  hSavedROMBank = hLoadedROMBank;

  // Switch audio ROM bank.
  hLoadedROMBank = wAudioROMBank;
  MBC1RomBank = wAudioROMBank;

  // Dispatch to correct audio engine.
  switch (wAudioROMBank)
  {
  case AUDIO1_BANK:
    Audio1_PlaySound(b);
    break;

  case AUDIO2_BANK:
    Audio2_PlaySound(b);
    break;

  default:
    Audio3_PlaySound(b);
    break;
  }

  // Restore previous ROM bank.
  hLoadedROMBank = hSavedROMBank;
  MBC1RomBank = hLoadedROMBank;
}

// ============================================================================
// PlayMusic
// ============================================================================

inline void PlayMusic(uint8_t sound, uint8_t bank)
{
  wNewSoundID = sound;

  wAudioFadeOutControl = 0;

  wAudioROMBank = bank;
  wAudioSavedROMBank = bank;

  PlaySound(sound);
}

// ============================================================================
// PlayDefaultMusicCommon
// ============================================================================

inline void PlayDefaultMusicCommon(
  uint8_t fadeOutLength,
  uint8_t fadeOutSpeed)
{
  uint8_t music;

  switch (wWalkBikeSurfState)
  {
    // Walking
  case 0:
  {
    music = wMapMusicSoundID;

    // If banks match and current music
    // already playing, do nothing.
    bool different =
      CompareMapMusicBankWithCurrentBank(
        fadeOutLength);

    if (!different &&
      wLastMusicSoundID == music)
    {
      return;
    }

    break;
  }

  // Surfing
  case 2:
  {
    music = MUSIC_SURFING;

    if (fadeOutSpeed == 0)
    {
      wAudioROMBank = AUDIO1_BANK;
    }

    wAudioSavedROMBank = AUDIO1_BANK;

    break;
  }

  // Biking
  default:
  {
    music = MUSIC_BIKE_RIDING;

    if (fadeOutSpeed == 0)
    {
      wAudioROMBank = AUDIO1_BANK;
    }

    wAudioSavedROMBank = AUDIO1_BANK;

    break;
  }
  }

  // Already playing?
  if (wLastMusicSoundID == music)
  {
    return;
  }

  wAudioFadeOutControl = fadeOutLength;

  wLastMusicSoundID = music;
  wNewSoundID = music;

  PlaySound(music);
}

// ============================================================================
// PlayDefaultMusic
// ============================================================================

inline void PlayDefaultMusic()
{
  WaitForSoundToFinish();

  wLastMusicSoundID = 0;

  PlayDefaultMusicCommon(
    0, // fade length
    0  // fade speed
  );
}

// ============================================================================
// PlayDefaultMusicFadeOutCurrent
// ============================================================================

inline void PlayDefaultMusicFadeOutCurrent()
{
  uint8_t fadeLength = 10;
  uint8_t fadeSpeed = 0;

  // Bit 5 = battle just ended
  if (wd72e & (1 << 5))
  {
    wLastMusicSoundID = 0;

    fadeLength = 8;
    fadeSpeed = 8;
  }

  PlayDefaultMusicCommon(
    fadeLength,
    fadeSpeed
  );
}

// ============================================================================
// UpdateMusic6Times
//
// Called when entering a map before fading out current music.
// ============================================================================

inline void UpdateMusic6Times()
{
  void (*updateFunc)() = nullptr;

  switch (wAudioROMBank)
  {
  case AUDIO1_BANK:
    updateFunc = Audio1_UpdateMusic;
    break;

  case AUDIO2_BANK:
    updateFunc = Audio2_UpdateMusic;
    break;

  default:
    updateFunc = Audio3_UpdateMusic;
    break;
  }

  for (uint8_t i = 0; i < 6; ++i)
  {
    Bankswitch();
    updateFunc();
  }
}