#define ASIO_STANDALONE
#include "game_room.hpp"
#include <asio.hpp>

namespace game {
game::GameState GameRoom::run_once(const game::ClientAction& action) {
    game::GameState state = {};
    state.status_code = -1;

    try {
        asio::io_context io_context;
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip_), port_);
        asio::ip::tcp::socket socket(io_context);

        socket.connect(endpoint);
        asio::write(socket, asio::buffer(&action, sizeof(game::ClientAction)));

        char reply[sizeof(game::GameState)];
        std::error_code ec;
        size_t length = socket.read_some(asio::buffer(reply), ec);

        if (!ec && length == sizeof(game::GameState)) {
            state = *reinterpret_cast<game::GameState*>(reply);
        }
    } catch (...) {}

    return state;
}
}
