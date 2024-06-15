#include "player.h"
#include "portal.h"
#include "maze.h"
#include "obstacle.h"
#include "empty.h"
#include <iostream>

player::player() : Block("player"), ATK(10), posX(1), posY(1), keysCollected(0), maze(nullptr)
{
    symbol = {{'P', 'P', 'P'}, {'P', 'P', 'P'}, {'P', 'P', 'P'}}; // 這沒卵用，原本檔案裡的
}

int player::getATK() const
{
    return ATK;
}

int player::GetK() const
{
    return keysCollected;
}
void player::resetKted()
{
    keysCollected = 0;
}

int player::getPosX() const
{
    return posX;
}

int player::getPosY() const
{
    return posY;
}

void player::setP(int x, int y)
{
    posX = x;
    posY = y;
}

void player::setMaze(std::vector<std::vector<std::unique_ptr<Block>>> &m)
{
    maze = &m;
}

void player::changeColor()
{
    color = sf::Color(rand() % 256, rand() % 256, rand() % 256);
}

sf::Color player::getColor() const
{
    return color;
}

void player::move(char direction)
{
    if (!maze)
        return;

    int newX = posX, newY = posY;
    if (direction == 'w' || direction == 'W')
        newX--;
    else if (direction == 's' || direction == 'S')
        newX++;
    else if (direction == 'a' || direction == 'A')
        newY--;
    else if (direction == 'd' || direction == 'D')
        newY++;

    if (newX >= 0 && newY >= 0 && newX < maze->size() && newY < (*maze)[0].size())
    {
        if ((*maze)[newX][newY]->getType() == "portal")
        {
            Portal *portal = dynamic_cast<Portal *>((*maze)[newX][newY].get());
            if (portal)
            {
                portal->isActivated = true;
                Portal *destination = portal->getLinkedPortal();
                if (destination)
                {
                    posX = destination->posX;
                    posY = destination->posY;
                }
            }
        }

        else if ((*maze)[newX][newY]->getType() != "wall" && (*maze)[newX][newY]->getType() != "obstacle")

        {
            if ((*maze)[newX][newY]->getType() == "key")
            {
                keysCollected++;
                (*maze)[newX][newY] = std::make_unique<Empty>(); // Change the position to empty
            }
            posX = newX;
            posY = newY;
            RVSRD();
            changeColor();
        }
    }
}

void player::RVSRD()
{
    if (!maze)
        return;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            int newX = posX + i, newY = posY + j;
            if (newX >= 0 && newY >= 0 && newX < maze->size() && newY < (*maze)[0].size())
            {
                (*maze)[newX][newY]->display();
            }
        }
    }
}

void player::displayST() const
{
    std::cout << "player ATK: " << ATK << std::endl;
    std::cout << "player Position: (" << posX << ", " << posY << ")" << std::endl;
    std::cout << "Keys Collected: " << keysCollected << std::endl;
}

void player::attack(std::unordered_map<std::tuple<int, int>, TileInfo, tuple_hash> &EXP_A)
{
    if (!maze)
        return;

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            int x = posX + dx;
            int y = posY + dy;
            if (x >= 0 && y >= 0 && x < maze->size() && y < (*maze)[0].size())
            {
                if ((*maze)[x][y]->getType() == "obstacle")
                {
                    Obstacle *obstacle = dynamic_cast<Obstacle *>((*maze)[x][y].get());
                    if (obstacle)
                    {
                        obstacle->takeDamage();
                        if (obstacle->isDestroyed())
                        {
                            (*maze)[x][y] = std::make_unique<Empty>();
                            EXP_A[std::make_tuple(x, y)] = {sf::Color::White, std::chrono::steady_clock::now()};
                        }
                    }
                }
            }
        }
    }
}