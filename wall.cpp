#include "wall.h"

Wall::Wall() : Block()
{
    symbol = {
        {'*', '*', '*'},
        {'*', '*', '*'},
        {'*', '*', '*'}};
    type = "wall";
}

void Wall::player_touched()
{
    std::cout << "you've touch the wall" << std::endl;
}
