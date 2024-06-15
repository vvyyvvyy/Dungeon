#include "portal.h"
#include <memory>

std::vector<Portal *> Portal::portals;

Portal::Portal(int id, int x, int y) : Block("portal"), posX(x), posY(y), isActivated(false), id(id), linkedPortal(nullptr)
{
    symbol = {{'P', 'P', 'P'}, {'P', 'P', 'P'}, {'P', 'P', 'P'}};
    portals.push_back(this);
}

int Portal::getID() const
{
    return id;
}

void Portal::linkPortals(const std::vector<std::vector<std::unique_ptr<Block>>> &maze)
{
    if (portals.size() % 2 != 0)
        return;

    for (size_t i = 0; i < portals.size(); i += 2)
    {
        Portal *p1 = portals[i];
        Portal *p2 = portals[i + 1];
        p1->linkedPortal = p2;
        p2->linkedPortal = p1;
    }
}

void Portal::player_touched()
{
    Portal *destination = getLinkedPortal();
    if (destination)
    {
        posX = destination->posX;
        posY = destination->posY;
    }
}

Portal *Portal::getLinkedPortal()
{
    return linkedPortal;
}