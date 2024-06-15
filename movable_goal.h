#ifndef MOVABLE_GOAL_H
#define MOVABLE_GOAL_H

#include "goal.h"
class MovableGoal : public Goal
{
public:
    MovableGoal();
    void move(std::vector<std::vector<std::unique_ptr<Block>>> &maze);
    MovableGoal(int x, int y);
    int getPosX() const;
    int getPosY() const;

private:
    int posX, posY;
};

#endif