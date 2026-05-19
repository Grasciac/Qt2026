#pragma once
#include <cstddef>

namespace game
{
    struct PlayerData
    {
        int id;
        int x;
        int y;
        char color[10];
        bool is_online;
    };

    struct GameState
    {
        int status_code;
        char weather[32];
        size_t players_count;
        PlayerData players[20];
    };

    struct ClientAction
    {
        int action_type;
        int id;
        int x;
        int y;
        char color[10];
    };
}