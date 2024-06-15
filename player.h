#ifndef player_H
#define player_H

#include <SFML/Graphics.hpp>
#include "block.h"
#include <vector>
#include <memory>
#include "maze.h"
#include "common.h"

class player : public Block
{
public:
    player();
    int getATK() const;
    void move(char direction);
    void displayST() const;
    int GetK() const;
    void resetKted();
    int getPosX() const;
    int getPosY() const;
    void setP(int x, int y);
    void attack(std::unordered_map<std::tuple<int, int>, TileInfo, tuple_hash> &EXP_A);
    void setMaze(std::vector<std::vector<std::unique_ptr<Block>>> &maze);
    void setPosition(int x, int y);
    void changeColor();
    sf::Color getColor() const;

private:
    int ATK;
    int posX, posY;
    int keysCollected;
    std::vector<std::vector<std::unique_ptr<Block>>> *maze;
    void RVSRD();
    sf::Color color;
};

#endif
