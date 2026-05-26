#include <cstdint>

// -----------------------------
// Hardware / globals (stubs)
// -----------------------------
uint8_t hLoadedROMBank;
uint8_t wVBlankSavedROMBank;

uint8_t hSCX, hSCY;
uint8_t rSCX, rSCY;

uint8_t hWY;
uint8_t rWY;

uint8_t wDisableVBlankWYUpdate;

uint8_t hVBlankOccurred;
uint8_t hFrameCounter;

uint8_t wAudioROMBank;
uint8_t MBC1RomBank;

constexpr uint8_t BANK_0 = 0;

// -----------------------------
// External routines (stubs)
// -----------------------------
void AutoBgMapTransfer();
void VBlankCopyBgMap();
void RedrawRowOrColumn();
void VBlankCopy();
void VBlankCopyDouble();
void UpdateMovingBgTiles();
void PrepareOAMData();
void Random();
void FadeOutAudio();
void ReadJoypad();

void Audio1_UpdateMusic();
void Audio2_UpdateMusic();
void Audio3_UpdateMusic();
void Music_DoLowHealthAlarm();

void TrackPlayTime();

// -----------------------------
// Bank switch helpers (stub)
// -----------------------------
void Bankswitch(uint8_t bank);

// -----------------------------
// VBlank
// -----------------------------
void VBlank() {
    uint8_t savedBank = hLoadedROMBank;
    wVBlankSavedROMBank = savedBank;

    rSCX = hSCX;
    rSCY = hSCY;

    if (!wDisableVBlankWYUpdate) {
        rWY = hWY;
    }

    AutoBgMapTransfer();
    VBlankCopyBgMap();
    RedrawRowOrColumn();
    VBlankCopy();
    VBlankCopyDouble();
    UpdateMovingBgTiles();
    PrepareOAMData();

    // VBlank-sensitive operations end.

    Random();

    if (hVBlankOccurred) {
        hVBlankOccurred = 0;
    }

    if (hFrameCounter) {
        hFrameCounter--;
    }

    FadeOutAudio();

    uint8_t audioBank = wAudioROMBank;
    hLoadedROMBank = audioBank;
    MBC1RomBank = audioBank;

    if (audioBank != BANK_0) {
        Audio3_UpdateMusic();
    }

    TrackPlayTime();

    if (!hVBlankOccurred) {
        ReadJoypad();
    }

    hLoadedROMBank = wVBlankSavedROMBank;
    MBC1RomBank = wVBlankSavedROMBank;
}

// -----------------------------
// DelayFrame
// -----------------------------
void DelayFrame() {
    constexpr uint8_t NOT_VBLANKED = 1;

    hVBlankOccurred = NOT_VBLANKED;

    while (hVBlankOccurred) {
        // equivalent to HALT waiting for VBlank interrupt
    }
}