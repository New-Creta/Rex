struct SFXChannel {
  int index;
  const char* data;
};

struct SFXHeader {
  int channelCount;
  std::vector<SFXChannel> channels;
};

const uint8_t SFX_Headers_1_Padding[3] = { 0xff, 0xff, 0xff };

SFXHeader SFX_Noise_Instrument01_1 = { 1, {{8, "SFX_Noise_Instrument01_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument02_1 = { 1, {{8, "SFX_Noise_Instrument02_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument03_1 = { 1, {{8, "SFX_Noise_Instrument03_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument04_1 = { 1, {{8, "SFX_Noise_Instrument04_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument05_1 = { 1, {{8, "SFX_Noise_Instrument05_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument06_1 = { 1, {{8, "SFX_Noise_Instrument06_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument07_1 = { 1, {{8, "SFX_Noise_Instrument07_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument08_1 = { 1, {{8, "SFX_Noise_Instrument08_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument09_1 = { 1, {{8, "SFX_Noise_Instrument09_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument10_1 = { 1, {{8, "SFX_Noise_Instrument10_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument11_1 = { 1, {{8, "SFX_Noise_Instrument11_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument12_1 = { 1, {{8, "SFX_Noise_Instrument12_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument13_1 = { 1, {{8, "SFX_Noise_Instrument13_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument14_1 = { 1, {{8, "SFX_Noise_Instrument14_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument15_1 = { 1, {{8, "SFX_Noise_Instrument15_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument16_1 = { 1, {{8, "SFX_Noise_Instrument16_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument17_1 = { 1, {{8, "SFX_Noise_Instrument17_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument18_1 = { 1, {{8, "SFX_Noise_Instrument18_1_Ch8"}} };
SFXHeader SFX_Noise_Instrument19_1 = { 1, {{8, "SFX_Noise_Instrument19_1_Ch8"}} };

SFXHeader SFX_Cry00_1 = { 3, {{5, "SFX_Cry00_1_Ch5"}, {6, "SFX_Cry00_1_Ch6"}, {8, "SFX_Cry00_1_Ch8"}} };
SFXHeader SFX_Cry01_1 = { 3, {{5, "SFX_Cry01_1_Ch5"}, {6, "SFX_Cry01_1_Ch6"}, {8, "SFX_Cry01_1_Ch8"}} };
SFXHeader SFX_Cry02_1 = { 3, {{5, "SFX_Cry02_1_Ch5"}, {6, "SFX_Cry02_1_Ch6"}, {8, "SFX_Cry02_1_Ch8"}} };
SFXHeader SFX_Cry03_1 = { 3, {{5, "SFX_Cry03_1_Ch5"}, {6, "SFX_Cry03_1_Ch6"}, {8, "SFX_Cry03_1_Ch8"}} };
SFXHeader SFX_Cry04_1 = { 3, {{5, "SFX_Cry04_1_Ch5"}, {6, "SFX_Cry04_1_Ch6"}, {8, "SFX_Cry04_1_Ch8"}} };
SFXHeader SFX_Cry05_1 = { 3, {{5, "SFX_Cry05_1_Ch5"}, {6, "SFX_Cry05_1_Ch6"}, {8, "SFX_Cry05_1_Ch8"}} };
SFXHeader SFX_Cry06_1 = { 3, {{5, "SFX_Cry06_1_Ch5"}, {6, "SFX_Cry06_1_Ch6"}, {8, "SFX_Cry06_1_Ch8"}} };
SFXHeader SFX_Cry07_1 = { 3, {{5, "SFX_Cry07_1_Ch5"}, {6, "SFX_Cry07_1_Ch6"}, {8, "SFX_Cry07_1_Ch8"}} };
SFXHeader SFX_Cry08_1 = { 3, {{5, "SFX_Cry08_1_Ch5"}, {6, "SFX_Cry08_1_Ch6"}, {8, "SFX_Cry08_1_Ch8"}} };
SFXHeader SFX_Cry09_1 = { 3, {{5, "SFX_Cry09_1_Ch5"}, {6, "SFX_Cry09_1_Ch6"}, {8, "SFX_Cry09_1_Ch8"}} };
SFXHeader SFX_Cry0A_1 = { 3, {{5, "SFX_Cry0A_1_Ch5"}, {6, "SFX_Cry0A_1_Ch6"}, {8, "SFX_Cry0A_1_Ch8"}} };
SFXHeader SFX_Cry0B_1 = { 3, {{5, "SFX_Cry0B_1_Ch5"}, {6, "SFX_Cry0B_1_Ch6"}, {8, "SFX_Cry0B_1_Ch8"}} };
SFXHeader SFX_Cry0C_1 = { 3, {{5, "SFX_Cry0C_1_Ch5"}, {6, "SFX_Cry0C_1_Ch6"}, {8, "SFX_Cry0C_1_Ch8"}} };
SFXHeader SFX_Cry0D_1 = { 3, {{5, "SFX_Cry0D_1_Ch5"}, {6, "SFX_Cry0D_1_Ch6"}, {8, "SFX_Cry0D_1_Ch8"}} };
SFXHeader SFX_Cry0E_1 = { 3, {{5, "SFX_Cry0E_1_Ch5"}, {6, "SFX_Cry0E_1_Ch6"}, {8, "SFX_Cry0E_1_Ch8"}} };
SFXHeader SFX_Cry0F_1 = { 3, {{5, "SFX_Cry0F_1_Ch5"}, {6, "SFX_Cry0F_1_Ch6"}, {8, "SFX_Cry0F_1_Ch8"}} };
SFXHeader SFX_Cry10_1 = { 3, {{5, "SFX_Cry10_1_Ch5"}, {6, "SFX_Cry10_1_Ch6"}, {8, "SFX_Cry10_1_Ch8"}} };
SFXHeader SFX_Cry11_1 = { 3, {{5, "SFX_Cry11_1_Ch5"}, {6, "SFX_Cry11_1_Ch6"}, {8, "SFX_Cry11_1_Ch8"}} };
SFXHeader SFX_Cry12_1 = { 3, {{5, "SFX_Cry12_1_Ch5"}, {6, "SFX_Cry12_1_Ch6"}, {8, "SFX_Cry12_1_Ch8"}} };
SFXHeader SFX_Cry13_1 = { 3, {{5, "SFX_Cry13_1_Ch5"}, {6, "SFX_Cry13_1_Ch6"}, {8, "SFX_Cry13_1_Ch8"}} };
SFXHeader SFX_Cry14_1 = { 3, {{5, "SFX_Cry14_1_Ch5"}, {6, "SFX_Cry14_1_Ch6"}, {8, "SFX_Cry14_1_Ch8"}} };
SFXHeader SFX_Cry15_1 = { 3, {{5, "SFX_Cry15_1_Ch5"}, {6, "SFX_Cry15_1_Ch6"}, {8, "SFX_Cry15_1_Ch8"}} };
SFXHeader SFX_Cry16_1 = { 3, {{5, "SFX_Cry16_1_Ch5"}, {6, "SFX_Cry16_1_Ch6"}, {8, "SFX_Cry16_1_Ch8"}} };
SFXHeader SFX_Cry17_1 = { 3, {{5, "SFX_Cry17_1_Ch5"}, {6, "SFX_Cry17_1_Ch6"}, {8, "SFX_Cry17_1_Ch8"}} };
SFXHeader SFX_Cry18_1 = { 3, {{5, "SFX_Cry18_1_Ch5"}, {6, "SFX_Cry18_1_Ch6"}, {8, "SFX_Cry18_1_Ch8"}} };
SFXHeader SFX_Cry19_1 = { 3, {{5, "SFX_Cry19_1_Ch5"}, {6, "SFX_Cry19_1_Ch6"}, {8, "SFX_Cry19_1_Ch8"}} };
SFXHeader SFX_Cry1A_1 = { 3, {{5, "SFX_Cry1A_1_Ch5"}, {6, "SFX_Cry1A_1_Ch6"}, {8, "SFX_Cry1A_1_Ch8"}} };
SFXHeader SFX_Cry1B_1 = { 3, {{5, "SFX_Cry1B_1_Ch5"}, {6, "SFX_Cry1B_1_Ch6"}, {8, "SFX_Cry1B_1_Ch8"}} };
SFXHeader SFX_Cry1C_1 = { 3, {{5, "SFX_Cry1C_1_Ch5"}, {6, "SFX_Cry1C_1_Ch6"}, {8, "SFX_Cry1C_1_Ch8"}} };
SFXHeader SFX_Cry1D_1 = { 3, {{5, "SFX_Cry1D_1_Ch5"}, {6, "SFX_Cry1D_1_Ch6"}, {8, "SFX_Cry1D_1_Ch8"}} };
SFXHeader SFX_Cry1E_1 = { 3, {{5, "SFX_Cry1E_1_Ch5"}, {6, "SFX_Cry1E_1_Ch6"}, {8, "SFX_Cry1E_1_Ch8"}} };
SFXHeader SFX_Cry1F_1 = { 3, {{5, "SFX_Cry1F_1_Ch5"}, {6, "SFX_Cry1F_1_Ch6"}, {8, "SFX_Cry1F_1_Ch8"}} };
SFXHeader SFX_Cry20_1 = { 3, {{5, "SFX_Cry20_1_Ch5"}, {6, "SFX_Cry20_1_Ch6"}, {8, "SFX_Cry20_1_Ch8"}} };
SFXHeader SFX_Cry21_1 = { 3, {{5, "SFX_Cry21_1_Ch5"}, {6, "SFX_Cry21_1_Ch6"}, {8, "SFX_Cry21_1_Ch8"}} };
SFXHeader SFX_Cry22_1 = { 3, {{5, "SFX_Cry22_1_Ch5"}, {6, "SFX_Cry22_1_Ch6"}, {8, "SFX_Cry22_1_Ch8"}} };
SFXHeader SFX_Cry23_1 = { 3, {{5, "SFX_Cry23_1_Ch5"}, {6, "SFX_Cry23_1_Ch6"}, {8, "SFX_Cry23_1_Ch8"}} };
SFXHeader SFX_Cry24_1 = { 3, {{5, "SFX_Cry24_1_Ch5"}, {6, "SFX_Cry24_1_Ch6"}, {8, "SFX_Cry24_1_Ch8"}} };
SFXHeader SFX_Cry25_1 = { 3, {{5, "SFX_Cry25_1_Ch5"}, {6, "SFX_Cry25_1_Ch6"}, {8, "SFX_Cry25_1_Ch8"}} };

SFXHeader SFX_Get_Item1_1 = { 3, {{5, "SFX_Get_Item1_1_Ch5"}, {6, "SFX_Get_Item1_1_Ch6"}, {7, "SFX_Get_Item1_1_Ch7"}} };
SFXHeader SFX_Get_Item2_1 = { 3, {{5, "SFX_Get_Item2_1_Ch5"}, {6, "SFX_Get_Item2_1_Ch6"}, {7, "SFX_Get_Item2_1_Ch7"}} };

SFXHeader SFX_Tink_1 = { 1, {{5, "SFX_Tink_1_Ch5"}} };
SFXHeader SFX_Heal_HP_1 = { 1, {{5, "SFX_Heal_HP_1_Ch5"}} };
SFXHeader SFX_Heal_Ailment_1 = { 1, {{5, "SFX_Heal_Ailment_1_Ch5"}} };
SFXHeader SFX_Start_Menu_1 = { 1, {{8, "SFX_Start_Menu_1_Ch8"}} };
SFXHeader SFX_Press_AB_1 = { 1, {{5, "SFX_Press_AB_1_Ch5"}} };

SFXHeader SFX_Pokedex_Rating_1 = {
    3,
    {
        {5, "SFX_Pokedex_Rating_1_Ch5"},
        {6, "SFX_Pokedex_Rating_1_Ch6"},
        {7, "SFX_Pokedex_Rating_1_Ch7"}
    }
};

SFXHeader SFX_Get_Key_Item_1 = {
    3,
    {
        {5, "SFX_Get_Key_Item_1_Ch5"},
        {6, "SFX_Get_Key_Item_1_Ch6"},
        {7, "SFX_Get_Key_Item_1_Ch7"}
    }
};

SFXHeader SFX_Poisoned_1 = { 1, {{5, "SFX_Poisoned_1_Ch5"}} };
SFXHeader SFX_Trade_Machine_1 = { 1, {{5, "SFX_Trade_Machine_1_Ch5"}} };
SFXHeader SFX_Turn_On_PC_1 = { 1, {{5, "SFX_Turn_On_PC_1_Ch5"}} };
SFXHeader SFX_Turn_Off_PC_1 = { 1, {{5, "SFX_Turn_Off_PC_1_Ch5"}} };
SFXHeader SFX_Enter_PC_1 = { 1, {{5, "SFX_Enter_PC_1_Ch5"}} };
SFXHeader SFX_Shrink_1 = { 1, {{5, "SFX_Shrink_1_Ch5"}} };
SFXHeader SFX_Switch_1 = { 1, {{5, "SFX_Switch_1_Ch5"}} };
SFXHeader SFX_Healing_Machine_1 = { 1, {{5, "SFX_Healing_Machine_1_Ch5"}} };
SFXHeader SFX_Teleport_Exit1_1 = { 1, {{5, "SFX_Teleport_Exit1_1_Ch5"}} };
SFXHeader SFX_Teleport_Enter1_1 = { 1, {{5, "SFX_Teleport_Enter1_1_Ch5"}} };
SFXHeader SFX_Teleport_Exit2_1 = { 1, {{5, "SFX_Teleport_Exit2_1_Ch5"}} };
SFXHeader SFX_Ledge_1 = { 1, {{5, "SFX_Ledge_1_Ch5"}} };
SFXHeader SFX_Teleport_Enter2_1 = { 1, {{8, "SFX_Teleport_Enter2_1_Ch8"}} };
SFXHeader SFX_Fly_1 = { 1, {{8, "SFX_Fly_1_Ch8"}} };

SFXHeader SFX_Denied_1 = {
    2,
    {
        {5, "SFX_Denied_1_Ch5"},
        {6, "SFX_Denied_1_Ch6"}
    }
};

SFXHeader SFX_Arrow_Tiles_1 = { 1, {{5, "SFX_Arrow_Tiles_1_Ch5"}} };
SFXHeader SFX_Push_Boulder_1 = { 1, {{8, "SFX_Push_Boulder_1_Ch8"}} };

SFXHeader SFX_SS_Anne_Horn_1 = {
    2,
    {
        {5, "SFX_SS_Anne_Horn_1_Ch5"},
        {6, "SFX_SS_Anne_Horn_1_Ch6"}
    }
};

SFXHeader SFX_Withdraw_Deposit_1 = { 1, {{5, "SFX_Withdraw_Deposit_1_Ch5"}} };
SFXHeader SFX_Cut_1 = { 1, {{8, "SFX_Cut_1_Ch8"}} };
SFXHeader SFX_Go_Inside_1 = { 1, {{8, "SFX_Go_Inside_1_Ch8"}} };

SFXHeader SFX_Swap_1 = {
    2,
    {
        {5, "SFX_Swap_1_Ch5"},
        {6, "SFX_Swap_1_Ch6"}
    }
};

SFXHeader SFX_59_1 = {
    2,
    {
        {5, "SFX_59_1_Ch5"},
        {6, "SFX_59_1_Ch6"}
    }
};

SFXHeader SFX_Purchase_1 = {
    2,
    {
        {5, "SFX_Purchase_1_Ch5"},
        {6, "SFX_Purchase_1_Ch6"}
    }
};

SFXHeader SFX_Collision_1 = { 1, {{5, "SFX_Collision_1_Ch5"}} };
SFXHeader SFX_Go_Outside_1 = { 1, {{8, "SFX_Go_Outside_1_Ch8"}} };

SFXHeader SFX_Save_1 = {
    2,
    {
        {5, "SFX_Save_1_Ch5"},
        {6, "SFX_Save_1_Ch6"}
    }
};

SFXHeader SFX_Pokeflute = {
    1,
    {
        {3, "SFX_Pokeflute_Ch3"}
    }
};

SFXHeader SFX_Safari_Zone_PA = {
    1,
    {
        {5, "SFX_Safari_Zone_PA_Ch5"}
    }
};