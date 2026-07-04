struct MusicChannel {
  int index;
  const char* data;
};

struct MusicTrack {
  int channelCount;
  std::vector<MusicChannel> channels;
};

MusicTrack Music_GymLeaderBattle = {
    3,
    {
        {1, "Music_GymLeaderBattle_Ch1"},
        {2, "Music_GymLeaderBattle_Ch2"},
        {3, "Music_GymLeaderBattle_Ch3"},
    }
};

MusicTrack Music_TrainerBattle = {
    3,
    {
        {1, "Music_TrainerBattle_Ch1"},
        {2, "Music_TrainerBattle_Ch2"},
        {3, "Music_TrainerBattle_Ch3"},
    }
};

MusicTrack Music_WildBattle = {
    3,
    {
        {1, "Music_WildBattle_Ch1"},
        {2, "Music_WildBattle_Ch2"},
        {3, "Music_WildBattle_Ch3"},
    }
};

MusicTrack Music_FinalBattle = {
    3,
    {
        {1, "Music_FinalBattle_Ch1"},
        {2, "Music_FinalBattle_Ch2"},
        {3, "Music_FinalBattle_Ch3"},
    }
};

MusicTrack Music_DefeatedTrainer = {
    3,
    {
        {1, "Music_DefeatedTrainer_Ch1"},
        {2, "Music_DefeatedTrainer_Ch2"},
        {3, "Music_DefeatedTrainer_Ch3"},
    }
};

MusicTrack Music_DefeatedWildMon = {
    3,
    {
        {1, "Music_DefeatedWildMon_Ch1"},
        {2, "Music_DefeatedWildMon_Ch2"},
        {3, "Music_DefeatedWildMon_Ch3"},
    }
};

MusicTrack Music_DefeatedGymLeader = {
    3,
    {
        {1, "Music_DefeatedGymLeader_Ch1"},
        {2, "Music_DefeatedGymLeader_Ch2"},
        {3, "Music_DefeatedGymLeader_Ch3"},
    }
};