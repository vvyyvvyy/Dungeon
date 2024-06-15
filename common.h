#ifndef COMMON_H
#define COMMON_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include <tuple>
#include <unordered_map>

struct tuple_hash
{
    template <typename T1, typename T2>
    std::size_t operator()(const std::tuple<T1, T2> &tpl) const
    {
        auto hash1 = std::hash<T1>{}(std::get<0>(tpl));
        auto hash2 = std::hash<T2>{}(std::get<1>(tpl));
        return hash1 ^ hash2;
    }
};

struct TileInfo
{
    sf::Color color;
    std::chrono::time_point<std::chrono::steady_clock> lastSeen;
};

#endif 
