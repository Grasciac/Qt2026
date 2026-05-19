#pragma once
#include <string>
#include "formats.hpp"

namespace game {
class GameRoom {
public:
    void set_address(const std::string& ip, unsigned short port) { ip_ = ip; port_ = port; }
    game::GameState run_once(const game::ClientAction& action);
private:
    std::string ip_;
    unsigned short port_ = 0;
};
}
