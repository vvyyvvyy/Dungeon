#include "obstacle.h"

Obstacle::Obstacle(int x, int y) : Empty(), health(5) { type = "obstacle"; }

void Obstacle::takeDamage()
{
    if (health > 0)
    {
        --health;
    }
}

bool Obstacle::isDestroyed() const
{
    return health <= 0;
}

int Obstacle::getHealth() const
{
    return health;
}