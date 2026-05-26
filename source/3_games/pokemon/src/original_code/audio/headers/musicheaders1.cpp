#include <cstdint>
#include <vector>
#include <string>

// ------------------------------------------------------------
// Music channel structures
// ------------------------------------------------------------

struct MusicChannel
{
  int id;
  const char* data;
};

struct MusicTrack
{
  const char* name;
  std::vector<MusicChannel> channels;
};

// ------------------------------------------------------------
// External channel data declarations
// ------------------------------------------------------------

extern const char* Music_PalletTown_Ch1;
extern const char* Music_PalletTown_Ch2;
extern const char* Music_PalletTown_Ch3;

extern const char* Music_Pokecenter_Ch1;
extern const char* Music_Pokecenter_Ch2;
extern const char* Music_Pokecenter_Ch3;

extern const char* Music_Gym_Ch1;
extern const char* Music_Gym_Ch2;
extern const char* Music_Gym_Ch3;

extern const char* Music_Cities1_Ch1;
extern const char* Music_Cities1_Ch2;
extern const char* Music_Cities1_Ch3;
extern const char* Music_Cities1_Ch4;

extern const char* Music_Cities2_Ch1;
extern const char* Music_Cities2_Ch2;
extern const char* Music_Cities2_Ch3;

extern const char* Music_Celadon_Ch1;
extern const char* Music_Celadon_Ch2;
extern const char* Music_Celadon_Ch3;

extern const char* Music_Cinnabar_Ch1;
extern const char* Music_Cinnabar_Ch2;
extern const char* Music_Cinnabar_Ch3;

extern const char* Music_Vermilion_Ch1;
extern const char* Music_Vermilion_Ch2;
extern const char* Music_Vermilion_Ch3;
extern const char* Music_Vermilion_Ch4;

extern const char* Music_Lavender_Ch1;
extern const char* Music_Lavender_Ch2;
extern const char* Music_Lavender_Ch3;
extern const char* Music_Lavender_Ch4;

extern const char* Music_SSAnne_Ch1;
extern const char* Music_SSAnne_Ch2;
extern const char* Music_SSAnne_Ch3;

extern const char* Music_MeetProfOak_Ch1;
extern const char* Music_MeetProfOak_Ch2;
extern const char* Music_MeetProfOak_Ch3;

extern const char* Music_MeetRival_Ch1;
extern const char* Music_MeetRival_Ch2;
extern const char* Music_MeetRival_Ch3;

extern const char* Music_MuseumGuy_Ch1;
extern const char* Music_MuseumGuy_Ch2;
extern const char* Music_MuseumGuy_Ch3;
extern const char* Music_MuseumGuy_Ch4;

extern const char* Music_SafariZone_Ch1;
extern const char* Music_SafariZone_Ch2;
extern const char* Music_SafariZone_Ch3;

extern const char* Music_PkmnHealed_Ch1;
extern const char* Music_PkmnHealed_Ch2;
extern const char* Music_PkmnHealed_Ch3;

extern const char* Music_Routes1_Ch1;
extern const char* Music_Routes1_Ch2;
extern const char* Music_Routes1_Ch3;
extern const char* Music_Routes1_Ch4;

extern const char* Music_Routes2_Ch1;
extern const char* Music_Routes2_Ch2;
extern const char* Music_Routes2_Ch3;
extern const char* Music_Routes2_Ch4;

extern const char* Music_Routes3_Ch1;
extern const char* Music_Routes3_Ch2;
extern const char* Music_Routes3_Ch3;
extern const char* Music_Routes3_Ch4;

extern const char* Music_Routes4_Ch1;
extern const char* Music_Routes4_Ch2;
extern const char* Music_Routes4_Ch3;
extern const char* Music_Routes4_Ch4;

extern const char* Music_IndigoPlateau_Ch1;
extern const char* Music_IndigoPlateau_Ch2;
extern const char* Music_IndigoPlateau_Ch3;
extern const char* Music_IndigoPlateau_Ch4;

// ------------------------------------------------------------
// Music definitions
// ------------------------------------------------------------

MusicTrack Music_PalletTown =
{
    "PalletTown",
    {
        {1, Music_PalletTown_Ch1},
        {2, Music_PalletTown_Ch2},
        {3, Music_PalletTown_Ch3}
    }
};

MusicTrack Music_Pokecenter =
{
    "Pokecenter",
    {
        {1, Music_Pokecenter_Ch1},
        {2, Music_Pokecenter_Ch2},
        {3, Music_Pokecenter_Ch3}
    }
};

MusicTrack Music_Gym =
{
    "Gym",
    {
        {1, Music_Gym_Ch1},
        {2, Music_Gym_Ch2},
        {3, Music_Gym_Ch3}
    }
};

MusicTrack Music_Cities1 =
{
    "Cities1",
    {
        {1, Music_Cities1_Ch1},
        {2, Music_Cities1_Ch2},
        {3, Music_Cities1_Ch3},
        {4, Music_Cities1_Ch4}
    }
};

MusicTrack Music_Cities2 =
{
    "Cities2",
    {
        {1, Music_Cities2_Ch1},
        {2, Music_Cities2_Ch2},
        {3, Music_Cities2_Ch3}
    }
};

MusicTrack Music_Celadon =
{
    "Celadon",
    {
        {1, Music_Celadon_Ch1},
        {2, Music_Celadon_Ch2},
        {3, Music_Celadon_Ch3}
    }
};

MusicTrack Music_Cinnabar =
{
    "Cinnabar",
    {
        {1, Music_Cinnabar_Ch1},
        {2, Music_Cinnabar_Ch2},
        {3, Music_Cinnabar_Ch3}
    }
};

MusicTrack Music_Vermilion =
{
    "Vermilion",
    {
        {1, Music_Vermilion_Ch1},
        {2, Music_Vermilion_Ch2},
        {3, Music_Vermilion_Ch3},
        {4, Music_Vermilion_Ch4}
    }
};

MusicTrack Music_Lavender =
{
    "Lavender",
    {
        {1, Music_Lavender_Ch1},
        {2, Music_Lavender_Ch2},
        {3, Music_Lavender_Ch3},
        {4, Music_Lavender_Ch4}
    }
};

MusicTrack Music_SSAnne =
{
    "SSAnne",
    {
        {1, Music_SSAnne_Ch1},
        {2, Music_SSAnne_Ch2},
        {3, Music_SSAnne_Ch3}
    }
};

MusicTrack Music_MeetProfOak =
{
    "MeetProfOak",
    {
        {1, Music_MeetProfOak_Ch1},
        {2, Music_MeetProfOak_Ch2},
        {3, Music_MeetProfOak_Ch3}
    }
};

MusicTrack Music_MeetRival =
{
    "MeetRival",
    {
        {1, Music_MeetRival_Ch1},
        {2, Music_MeetRival_Ch2},
        {3, Music_MeetRival_Ch3}
    }
};

MusicTrack Music_MuseumGuy =
{
    "MuseumGuy",
    {
        {1, Music_MuseumGuy_Ch1},
        {2, Music_MuseumGuy_Ch2},
        {3, Music_MuseumGuy_Ch3},
        {4, Music_MuseumGuy_Ch4}
    }
};

MusicTrack Music_SafariZone =
{
    "SafariZone",
    {
        {1, Music_SafariZone_Ch1},
        {2, Music_SafariZone_Ch2},
        {3, Music_SafariZone_Ch3}
    }
};

MusicTrack Music_PkmnHealed =
{
    "PkmnHealed",
    {
        {1, Music_PkmnHealed_Ch1},
        {2, Music_PkmnHealed_Ch2},
        {3, Music_PkmnHealed_Ch3}
    }
};

MusicTrack Music_Routes1 =
{
    "Routes1",
    {
        {1, Music_Routes1_Ch1},
        {2, Music_Routes1_Ch2},
        {3, Music_Routes1_Ch3},
        {4, Music_Routes1_Ch4}
    }
};

MusicTrack Music_Routes2 =
{
    "Routes2",
    {
        {1, Music_Routes2_Ch1},
        {2, Music_Routes2_Ch2},
        {3, Music_Routes2_Ch3},
        {4, Music_Routes2_Ch4}
    }
};

MusicTrack Music_Routes3 =
{
    "Routes3",
    {
        {1, Music_Routes3_Ch1},
        {2, Music_Routes3_Ch2},
        {3, Music_Routes3_Ch3},
        {4, Music_Routes3_Ch4}
    }
};

MusicTrack Music_Routes4 =
{
    "Routes4",
    {
        {1, Music_Routes4_Ch1},
        {2, Music_Routes4_Ch2},
        {3, Music_Routes4_Ch3},
        {4, Music_Routes4_Ch4}
    }
};

MusicTrack Music_IndigoPlateau =
{
    "IndigoPlateau",
    {
        {1, Music_IndigoPlateau_Ch1},
        {2, Music_IndigoPlateau_Ch2},
        {3, Music_IndigoPlateau_Ch3},
        {4, Music_IndigoPlateau_Ch4}
    }
};