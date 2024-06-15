#include "movable_goal.h"
#include "empty.h"
#include <cstdlib>

MovableGoal::MovableGoal(int x, int y) : Goal(), posX(x), posY(y) {}

void MovableGoal::move(std::vector<std::vector<std::unique_ptr<Block>>> &maze)
{
    int direction = rand() % 4;
    int newX = posX, newY = posY;
    if (direction == 0)
        newX--;
    else if (direction == 1)
        newX++;
    else if (direction == 2)
        newY--;
    else if (direction == 3)
        newY++;

    if (newX >= 0 && newX < maze.size() && newY >= 0 && newY < maze[newX].size() && maze[newX][newY]->getType() == "empty")
    {
        maze[posX][posY] = std::make_unique<Empty>();
        maze[newX][newY] = std::make_unique<MovableGoal>(newX, newY);
        posX = newX;
        posY = newY;
    }
}

int MovableGoal::getPosX() const
{
    return posX;
}

int MovableGoal::getPosY() const
{
    return posY;
}