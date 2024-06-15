#ifndef MAZE_H
#define MAZE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <tuple>
#include "block.h"
#include "obstacle.h"
#include "player.h"
#include "common.h"

class player;
class Maze
{
public:
    void moveGoal();
    void PlaceO(int ct);
    void setShowTLPConfirmation(bool show);
    bool GStele() const;
    void tryTLP(int playerX, int playerY);
    void drawTLPConfirmation(sf::RenderWindow &window) const;
    Maze(const std::string &filename);
    void display(sf::RenderWindow &window, const player &player, const std::unordered_map<std::tuple<int, int>, TileInfo, tuple_hash> &EXP_A) const;
    std::vector<std::vector<std::unique_ptr<Block>>> &getGR();

private:
    std::vector<std::vector<std::unique_ptr<Block>>> grid;
    void loadMaze(const std::string &filename);
    bool showTLPConfirmation;
};

#endif
