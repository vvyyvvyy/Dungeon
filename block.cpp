#include "block.h"

Block::Block() : type("invisible"), symbol(3, std::vector<char>(3, ' ')) {}

Block::Block(const std::string &type) : type(type), symbol(3, std::vector<char>(3, ' '))
{
    if (type == "wall")
    {
        symbol = {{'#', '#', '#'}, {'#', '#', '#'}, {'#', '#', '#'}};
    }
    else if (type == "empty")
    {
        symbol = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    }
    else if (type == "goal")
    {
        symbol = {{'G', 'G', 'G'}, {'G', 'G', 'G'}, {'G', 'G', 'G'}};
    }
}

Block::~Block() {}

std::string Block::getType() const
{
    return type;
}

void Block::setType(const std::string &type)
{
    this->type = type;
}

void Block::display() const
{
    for (const auto &row : symbol)
    {
        for (char c : row)
        {
        }
    }
}

const std::vector<std::vector<char>> &Block::getSymbol() const
{
    return symbol;
}

void Block::player_touched()
{
    // TODO
}
