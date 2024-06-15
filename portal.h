#ifndef PORTAL_H
#define PORTAL_H

#include "block.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Portal : public Block
{
public:
    Portal(int id, int x, int y);
    int getID() const;
    static void linkPortals(const std::vector<std::vector<std::unique_ptr<Block>>> &maze);
    void player_touched() override;
    Portal *getLinkedPortal();
    sf::Clock activationClock;

    int posX, posY;
    bool isActivated;

private:
    int id;
    Portal *linkedPortal;
    static std::vector<Portal *> portals;
};

#endif