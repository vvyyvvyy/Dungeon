#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "empty.h"
class Obstacle : public Empty
{
public:
    Obstacle(int x, int y);
    void takeDamage();
    bool isDestroyed() const;
    int getHealth() const;
   
private:
    int health;
};

#endif