#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <string>
#include <vector>

class Block
{
public:
    Block();
    Block(const std::string &type);
    virtual ~Block();
    std::string getType() const;
    void setType(const std::string &type);
    virtual void display() const;
    const std::vector<std::vector<char>> &getSymbol() const;
    virtual void player_touched();

protected:
    std::string type;
    std::vector<std::vector<char>> symbol;
};

#endif
