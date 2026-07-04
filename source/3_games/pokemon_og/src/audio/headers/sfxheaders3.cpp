#pragma once
#include <cstdint>

namespace SFX3
{
  constexpr uint8_t headers_padding[3] = { 0xFF, 0xFF, 0xFF };

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
  // Noise instruments (1 channel, ch8)
  // -------------------------
  extern const SFXDef Noise_Instrument01_3;
  extern const SFXDef Noise_Instrument02_3;
  extern const SFXDef Noise_Instrument03_3;
  extern const SFXDef Noise_Instrument04_3;
  extern const SFXDef Noise_Instrument05_3;
  extern const SFXDef Noise_Instrument06_3;
  extern const SFXDef Noise_Instrument07_3;
  extern const SFXDef Noise_Instrument08_3;
  extern const SFXDef Noise_Instrument09_3;
  extern const SFXDef Noise_Instrument10_3;
  extern const SFXDef Noise_Instrument11_3;
  extern const SFXDef Noise_Instrument12_3;
  extern const SFXDef Noise_Instrument13_3;
  extern const SFXDef Noise_Instrument14_3;
  extern const SFXDef Noise_Instrument15_3;
  extern const SFXDef Noise_Instrument16_3;
  extern const SFXDef Noise_Instrument17_3;
  extern const SFXDef Noise_Instrument18_3;
  extern const SFXDef Noise_Instrument19_3;

  // -------------------------
  // Cries (3 channels: 5,6,8)
  // -------------------------
  extern const SFXDef Cry00_3;
  extern const SFXDef Cry01_3;
  extern const SFXDef Cry02_3;
  extern const SFXDef Cry03_3;
  extern const SFXDef Cry04_3;
  extern const SFXDef Cry05_3;
  extern const SFXDef Cry06_3;
  extern const SFXDef Cry07_3;
  extern const SFXDef Cry08_3;
  extern const SFXDef Cry09_3;
  extern const SFXDef Cry0A_3;
  extern const SFXDef Cry0B_3;
  extern const SFXDef Cry0C_3;
  extern const SFXDef Cry0D_3;
  extern const SFXDef Cry0E_3;
  extern const SFXDef Cry0F_3;
  extern const SFXDef Cry10_3;
  extern const SFXDef Cry11_3;
  extern const SFXDef Cry12_3;
  extern const SFXDef Cry13_3;
  extern const SFXDef Cry14_3;
  extern const SFXDef Cry15_3;
  extern const SFXDef Cry16_3;
  extern const SFXDef Cry17_3;
  extern const SFXDef Cry18_3;
  extern const SFXDef Cry19_3;
  extern const SFXDef Cry1A_3;
  extern const SFXDef Cry1B_3;
  extern const SFXDef Cry1C_3;
  extern const SFXDef Cry1D_3;
  extern const SFXDef Cry1E_3;
  extern const SFXDef Cry1F_3;
  extern const SFXDef Cry20_3;
  extern const SFXDef Cry21_3;
  extern const SFXDef Cry22_3;
  extern const SFXDef Cry23_3;
  extern const SFXDef Cry24_3;
  extern const SFXDef Cry25_3;

  // -------------------------
  // Misc SFX
  // -------------------------
  extern const SFXDef Get_Item1_3;
  extern const SFXDef Get_Item2_3;
  extern const SFXDef Tink_3;
  extern const SFXDef Heal_HP_3;
  extern const SFXDef Heal_Ailment_3;
  extern const SFXDef Start_Menu_3;
  extern const SFXDef Press_AB_3;
  extern const SFXDef Pokedex_Rating_3;
  extern const SFXDef Get_Key_Item_3;
  extern const SFXDef Poisoned_3;
  extern const SFXDef Trade_Machine_3;
  extern const SFXDef Turn_On_PC_3;
  extern const SFXDef Turn_Off_PC_3;
  extern const SFXDef Enter_PC_3;
  extern const SFXDef Shrink_3;
  extern const SFXDef Switch_3;
  extern const SFXDef Healing_Machine_3;
  extern const SFXDef Teleport_Exit1_3;
  extern const SFXDef Teleport_Enter1_3;
  extern const SFXDef Teleport_Exit2_3;
  extern const SFXDef Ledge_3;
  extern const SFXDef Teleport_Enter2_3;
  extern const SFXDef Fly_3;
  extern const SFXDef Denied_3;
  extern const SFXDef Arrow_Tiles_3;
  extern const SFXDef Push_Boulder_3;
  extern const SFXDef SS_Anne_Horn_3;
  extern const SFXDef Withdraw_Deposit_3;
  extern const SFXDef Cut_3;
  extern const SFXDef Go_Inside_3;
  extern const SFXDef Swap_3;
  extern const SFXDef SFX_59_3;
  extern const SFXDef Purchase_3;
  extern const SFXDef Collision_3;
  extern const SFXDef Go_Outside_3;
  extern const SFXDef Save_3;

  // -------------------------
  // Intro / slots / misc
  // -------------------------
  extern const SFXDef Intro_Lunge;
  extern const SFXDef Intro_Hip;
  extern const SFXDef Intro_Hop;
  extern const SFXDef Intro_Raise;
  extern const SFXDef Intro_Crash;
  extern const SFXDef Intro_Whoosh;

  extern const SFXDef Slots_Stop_Wheel;
  extern const SFXDef Slots_Reward;
  extern const SFXDef Slots_New_Spin;
  extern const SFXDef Shooting_Star;
}

// -------------------------
// Example definitions (pattern only)
// -------------------------

static const SFX3::ChannelRef SFX_Cry00_3_channels[] = {
    {5, SFX_Cry00_3_Ch5},
    {6, SFX_Cry00_3_Ch6},
    {8, SFX_Cry00_3_Ch8},
};

static const SFX3::SFXDef SFX3::Cry00_3 = {
    3,
    SFX_Cry00_3_channels
};

static const SFX3::ChannelRef SFX_Start_Menu_3_channels[] = {
    {8, SFX_Start_Menu_3_Ch8},
};

static const SFX3::SFXDef SFX3::Start_Menu_3 = {
    1,
    SFX_Start_Menu_3_channels
};