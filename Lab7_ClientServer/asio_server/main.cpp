#define _CRT_SECURE_NO_WARNINGS
#define ASIO_STANDALONE
#include <asio.hpp>
#include <iostream>
#include <map>
#include <cstring>
#include <chrono>
#include <random>
#include <fstream>
#include <csignal>
#include <nlohmann/json.hpp>
#include <windows.h>
#include "../common/formats.hpp"

using asio::ip::tcp;
using json = nlohmann::json;

struct ServerPlayerSession {
    int id;
    int x;
    int y;
    std::string color;
    bool is_online;
    std::chrono::steady_clock::time_point last_active;
};

std::map<int, ServerPlayerSession> session_db;

void save_and_exit(int signal) {
    std::ofstream file("clients_data.json");
    if (!session_db.empty()) {
        json j = json::array();
        for (const auto& [id, session] : session_db) {
            j.push_back({
                {"id", id},
                {"color", session.color},
                {"x", session.x},
                {"y", session.y}
            });
        }
        file << j.dump(4);
    }
    std::cout << "\nДанные сохранены. Сервер остановлен.\n";
    std::exit(signal);
}

void handle_client(tcp::socket socket) {
    try {
        game::ClientAction req;
        std::error_code ec;

        asio::read(socket, asio::buffer(&req, sizeof(game::ClientAction)), ec);
        if (ec) return;

        game::GameState res = {};
        res.status_code = 0;
        auto now = std::chrono::steady_clock::now();

        // Удаляем старых неактивных клиентов (старше 30 секунд)
        for (auto it = session_db.begin(); it != session_db.end(); ) {
            if (!it->second.is_online &&
                std::chrono::duration_cast<std::chrono::seconds>(now - it->second.last_active).count() > 30) {
                it = session_db.erase(it);
            } else {
                ++it;
            }
        }

        if (req.action_type == 1) { // AUTH
            // Проверяем только ОНЛАЙН игроков
            if (session_db.count(req.id) && session_db[req.id].is_online) {
                res.status_code = 1; // ID занят
            } else {
                // Регистрируем или восстанавливаем
                session_db[req.id] = {req.id, 0, 0, req.color, true, now};
            }
        }
        else if (req.action_type == 2) { // MOVE
            if (session_db.count(req.id)) {
                session_db[req.id].x = req.x;
                session_db[req.id].y = req.y;
                session_db[req.id].color = req.color;
                session_db[req.id].is_online = true;
                session_db[req.id].last_active = now;
            }
        }
        else if (req.action_type == 0) { // POLL
            if (session_db.count(req.id)) {
                session_db[req.id].is_online = true;
                session_db[req.id].last_active = now;
            }
        }

        // Формируем список только онлайн игроков
        res.players_count = 0;
        for (const auto& [id, session] : session_db) {
            if (session.is_online && res.players_count < 20) {
                auto& p = res.players[res.players_count++];
                p.id = session.id;
                p.x = session.x;
                p.y = session.y;
                p.is_online = true;
                std::strcpy(p.color, session.color.c_str());
            }
        }

        // Погода (простая реализация)
        static const char* weather_list[] = {"Ясно", "Пасмурно", "Дождливо"};
        static int weather_idx = 0;
        static auto last_weather = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_weather).count() >= 10) {
            weather_idx = (weather_idx + 1) % 3;
            last_weather = now;
        }
        std::strcpy(res.weather, weather_list[weather_idx]);

        asio::write(socket, asio::buffer(&res, sizeof(game::GameState)), ec);

        // После ответа помечаем как offline (клиент отключился)
        if (session_db.count(req.id) && req.action_type != 2 && req.action_type != 0) {
            session_db[req.id].is_online = false;
        }

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::signal(SIGINT, save_and_exit);
    std::signal(SIGTERM, save_and_exit);

    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 7000));
        std::cout << "Сервер запущен на порту 7000...\n";
        std::cout << "Нажмите Ctrl+C для остановки\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            handle_client(std::move(socket));
        }

    } catch (const std::exception& ex) {
        std::cerr << "Исключение: " << ex.what() << std::endl;
    }

    return 0;
}
