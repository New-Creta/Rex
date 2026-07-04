#pragma once
#include <cstdint>

namespace SFX
{
  constexpr uint8_t padding_headers_2[3] = { 0xFF, 0xFF, 0xFF };

  struct ChannelRef
  {
    uint8_t channel;
    const void* ptr;
  };

  struct SFXDef
  {
    uint8_t channelCount;
    const ChannelRef* channels;
  };

  // -------------------------
  // Noise instruments (1 channel each, channel 8)
  // -------------------------

  extern const SFXDef Noise_Instrument01_2;
  extern const SFXDef Noise_Instrument02_2;
  extern const SFXDef Noise_Instrument03_2;
  extern const SFXDef Noise_Instrument04_2;
  extern const SFXDef Noise_Instrument05_2;
  extern const SFXDef Noise_Instrument06_2;
  extern const SFXDef Noise_Instrument07_2;
  extern const SFXDef Noise_Instrument08_2;
  extern const SFXDef Noise_Instrument09_2;
  extern const SFXDef Noise_Instrument10_2;
  extern const SFXDef Noise_Instrument11_2;
  extern const SFXDef Noise_Instrument12_2;
  extern const SFXDef Noise_Instrument13_2;
  extern const SFXDef Noise_Instrument14_2;
  extern const SFXDef Noise_Instrument15_2;
  extern const SFXDef Noise_Instrument16_2;
  extern const SFXDef Noise_Instrument17_2;
  extern const SFXDef Noise_Instrument18_2;
  extern const SFXDef Noise_Instrument19_2;

  // -------------------------
  // Cry SFX (3 channels: 5,6,8)
  // -------------------------

  extern const SFXDef Cry00_2;
  extern const SFXDef Cry01_2;
  extern const SFXDef Cry02_2;
  extern const SFXDef Cry03_2;
  extern const SFXDef Cry04_2;
  extern const SFXDef Cry05_2;
  extern const SFXDef Cry06_2;
  extern const SFXDef Cry07_2;
  extern const SFXDef Cry08_2;
  extern const SFXDef Cry09_2;
  extern const SFXDef Cry0A_2;
  extern const SFXDef Cry0B_2;
  extern const SFXDef Cry0C_2;
  extern const SFXDef Cry0D_2;
  extern const SFXDef Cry0E_2;
  extern const SFXDef Cry0F_2;
  extern const SFXDef Cry10_2;
  extern const SFXDef Cry11_2;
  extern const SFXDef Cry12_2;
  extern const SFXDef Cry13_2;
  extern const SFXDef Cry14_2;
  extern const SFXDef Cry15_2;
  extern const SFXDef Cry16_2;
  extern const SFXDef Cry17_2;
  extern const SFXDef Cry18_2;
  extern const SFXDef Cry19_2;
  extern const SFXDef Cry1A_2;
  extern const SFXDef Cry1B_2;
  extern const SFXDef Cry1C_2;
  extern const SFXDef Cry1D_2;
  extern const SFXDef Cry1E_2;
  extern const SFXDef Cry1F_2;
  extern const SFXDef Cry20_2;
  extern const SFXDef Cry21_2;
  extern const SFXDef Cry22_2;
  extern const SFXDef Cry23_2;
  extern const SFXDef Cry24_2;
  extern const SFXDef Cry25_2;

  // -------------------------
  // Misc SFX
  // -------------------------

  extern const SFXDef Level_Up;
  extern const SFXDef Get_Item2_2;
  extern const SFXDef Tink_2;
  extern const SFXDef Heal_HP_2;
  extern const SFXDef Heal_Ailment_2;
  extern const SFXDef Start_Menu_2;
  extern const SFXDef Press_AB_2;
  extern const SFXDef Ball_Toss;
  extern const SFXDef Ball_Poof;
  extern const SFXDef Faint_Thud;
  extern const SFXDef Run;
  extern const SFXDef Dex_Page_Added;
  extern const SFXDef Caught_Mon;
  extern const SFXDef Peck;
  extern const SFXDef Faint_Fall;
  extern const SFXDef Battle_09;
  extern const SFXDef Pound;
  extern const SFXDef Battle_0B;
  extern const SFXDef Battle_0C;
  extern const SFXDef Battle_0D;
  extern const SFXDef Battle_0E;
  extern const SFXDef Battle_0F;
  extern const SFXDef Damage;
  extern const SFXDef Not_Very_Effective;
  extern const SFXDef Battle_12;
  extern const SFXDef Battle_13;
  extern const SFXDef Battle_14;
  extern const SFXDef Vine_Whip;
  extern const SFXDef Battle_16;
  extern const SFXDef Battle_17;
  extern const SFXDef Battle_18;
  extern const SFXDef Battle_19;
  extern const SFXDef Super_Effective;
  extern const SFXDef Battle_1B;
  extern const SFXDef Battle_1C;
  extern const SFXDef Doubleslap;
  extern const SFXDef Battle_1E;
  extern const SFXDef Horn_Drill;
  extern const SFXDef Battle_20;
  extern const SFXDef Battle_21;
  extern const SFXDef Battle_22;
  extern const SFXDef Battle_23;
  extern const SFXDef Battle_24;
  extern const SFXDef Battle_25;
  extern const SFXDef Battle_26;
  extern const SFXDef Battle_27;
  extern const SFXDef Battle_28;
  extern const SFXDef Battle_29;
  extern const SFXDef Battle_2A;
  extern const SFXDef Battle_2B;
  extern const SFXDef Battle_2C;
  extern const SFXDef Psybeam;
  extern const SFXDef Battle_2E;
  extern const SFXDef Battle_2F;
  extern const SFXDef Psychic_M;
  extern const SFXDef Battle_31;
  extern const SFXDef Battle_32;
  extern const SFXDef Battle_33;
  extern const SFXDef Battle_34;
  extern const SFXDef Battle_35;
  extern const SFXDef Battle_36;
  extern const SFXDef Silph_Scope;
}

// -------------------------
// Example static definitions
// (pattern shown; repeat for all)
// -------------------------

static const SFX::ChannelRef SFX_Cry00_2_channels[] = {
    {5, SFX_Cry00_2_Ch5},
    {6, SFX_Cry00_2_Ch6},
    {8, SFX_Cry00_2_Ch8},
};

static const SFX::SFXDef SFX::Cry00_2 = {
    3,
    SFX_Cry00_2_channels
};

static const SFX::ChannelRef SFX_Run_channels[] = {
    {8, SFX_Run_Ch8},
};

static const SFX::SFXDef SFX::Run = {
    1,
    SFX_Run_channels
};