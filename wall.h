#ifndef WALL_H
#define WALL_H

#include "block.h"

class Wall : public Block
{
public:
    Wall();
    void player_touched() override;
};

#endif
