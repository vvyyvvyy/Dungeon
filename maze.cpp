#include "maze.h"
#include "block.h"
#include "empty.h"
#include "goal.h"
#include "key.h"
#include "player.h"
#include "portal.h"
#include "obstacle.h"
#include "movable_goal.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <random>
#include <algorithm>

Maze::Maze(const std::string &filename) : showTLPConfirmation(false)
{
    loadMaze(filename);
}

void Maze::loadMaze(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open the maze file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::unique_ptr<Block>> row;
        for (int j = 0; j < line.length(); ++j)
        {
            char c = line[j];
            if (c == '1')
            {
                row.push_back(std::make_unique<Block>("wall"));
            }
            else if (c == '0')
            {
                row.push_back(std::make_unique<Block>("empty"));
            }
            else if (c == '2')
            {
                if (filename == "maze3.txt" || filename == "maze4.txt")
                {
                    row.push_back(std::make_unique<MovableGoal>(grid.size(), j));
                }
                else
                {
                    row.push_back(std::make_unique<Goal>());
                }
            }
            else
            {
                row.push_back(std::make_unique<Block>("invisible"));
            }
        }
        grid.push_back(std::move(row));
    }

    file.close();

    std::vector<std::pair<int, int>> EPS;
    for (int i = 0; i < grid.size(); ++i)
    {
        for (int j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j]->getType() == "empty")
            {

                EPS.emplace_back(i, j);
            }
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(EPS.begin(), EPS.end(), rng);

    int numPortals = 2;
    for (int i = 0; i < numPortals && i < EPS.size(); ++i)
    {
        int x = EPS[i].first;
        int y = EPS[i].second;
        grid[x][y] = std::make_unique<Portal>(i, x, y);
    }

    Portal::linkPortals(grid);
}

sf::Color getObsC(int health, int maxHealth)
{
    int red = 165;
    int green = 42;
    int blue = 42;
    int maxDifference = 255 - red;

    float healthRatio = static_cast<float>(health) / maxHealth;
    int difference = static_cast<int>(maxDifference * (1 - healthRatio));

    return sf::Color(red + difference, green + difference, blue + difference);
}

std::vector<std::vector<std::unique_ptr<Block>>> &Maze::getGR()
{
    return grid;
}

void Maze::display(sf::RenderWindow &window, const player &player, const std::unordered_map<std::tuple<int, int>, TileInfo, tuple_hash> &EXP_A) const
{
    for (int i = 0; i < grid.size(); ++i)
    {
        for (int j = 0; j < grid[i].size(); ++j)
        {
            sf::RectangleShape blockShape(sf::Vector2f(30, 30));
            blockShape.setPosition(j * 30, i * 30);

            auto it = EXP_A.find(std::make_tuple(i, j));
            if (it != EXP_A.end())
            {
                blockShape.setFillColor(it->second.color);
            }
            else
            {
                blockShape.setFillColor(sf::Color::Black);
            }

            if (grid[i][j]->getType() == "obstacle")
            {
                Obstacle *obstacle = dynamic_cast<Obstacle *>(grid[i][j].get());
                if (obstacle)
                {

#ifdef obstacle_debug
                    std::cout << "Found obstacle at (" << i << ", " << j << ")" << std::endl;
#endif

                    int health = obstacle->getHealth();
                    int maxHealth = 5;
                    int playerDistance = std::abs(player.getPosX() - i) + std::abs(player.getPosY() - j);
                    int maxDistance = 1;

                    if (playerDistance <= maxDistance)
                    {
                        blockShape.setFillColor(getObsC(health, maxHealth));
                    }
                    else
                    {
                        blockShape.setFillColor(sf::Color::Black);
                    }
                }
            }

            if (grid[i][j]->getType() == "portal")
            {
                Portal *portal = dynamic_cast<Portal *>(grid[i][j].get());
                if (portal)
                {
                    if (portal->isActivated)
                    {

                        float time = portal->activationClock.getElapsedTime().asSeconds();
                        float alpha = std::sin(time * 3.14159f) * 127 + 128;
                        blockShape.setFillColor(sf::Color(255, 128, 0, static_cast<sf::Uint8>(alpha)));
                    }
                    else
                    {

                        blockShape.setFillColor(sf::Color::Black);
                    }
                }
            }

            if (grid[i][j]->getType() == "goal")
            {
                MovableGoal *movableGoal = dynamic_cast<MovableGoal *>(grid[i][j].get());
                if (movableGoal)
                {
                    sf::RectangleShape goalShape(sf::Vector2f(30, 30));
                    goalShape.setPosition(movableGoal->getPosY() * 30, movableGoal->getPosX() * 30);

                    int playerDistance = std::abs(player.getPosX() - movableGoal->getPosX()) + std::abs(player.getPosY() - movableGoal->getPosY());
                    int maxDistance = 1;

                    if (playerDistance <= maxDistance)
                    {
                        goalShape.setFillColor(sf::Color::Green);
                    }
                    else
                    {
                        goalShape.setFillColor(sf::Color::Black);
                    }

                    window.draw(goalShape);
                }
                else
                {
                    Goal *goal = dynamic_cast<Goal *>(grid[i][j].get());
                    if (goal)
                    {
                        int playerDistance = std::abs(player.getPosX() - i) + std::abs(player.getPosY() - j);
                        int maxDistance = 1;

                        if (playerDistance <= maxDistance)
                        {
                            blockShape.setFillColor(sf::Color::Green);
                        }
                        else
                        {
                            blockShape.setFillColor(sf::Color::Black);
                        }
                    }
                }
            }

            window.draw(blockShape);
        }
    }

    sf::RectangleShape playerShape(sf::Vector2f(30, 30));
    playerShape.setPosition(player.getPosY() * 30, player.getPosX() * 30);

#ifdef player_color_will_change
    playerShape.setFillColor(player.getColor());
#endif

#ifndef player_color_will_change
    playerShape.setFillColor(sf::Color::Red);
#endif
    window.draw(playerShape);
}

void Maze::setShowTLPConfirmation(bool show)
{
    showTLPConfirmation = show;
}

bool Maze::GStele() const
{
    return showTLPConfirmation;
}

void Maze::drawTLPConfirmation(sf::RenderWindow &window) const
{
    if (showTLPConfirmation)
    {
        sf::View currentView = window.getView();
        sf::Vector2f viewCenter = currentView.getCenter();

        sf::RectangleShape backgroundShape(sf::Vector2f(200, 100));
        backgroundShape.setFillColor(sf::Color(0, 0, 0, 200));
        backgroundShape.setPosition(viewCenter.x - 100, viewCenter.y - 50);

        sf::Text TX;
        sf::Font font;
        if (font.loadFromFile("Silkscreen-Regular.ttf"))
        {
            TX.setFont(font);
            TX.setString("TLP?");
            TX.setCharacterSize(24);
            TX.setFillColor(sf::Color::White);
            TX.setPosition(viewCenter.x - TX.getLocalBounds().width / 2, viewCenter.y - TX.getLocalBounds().height / 2);
        }

        window.draw(backgroundShape);
        window.draw(TX);
    }
}

void Maze::tryTLP(int playerX, int playerY)
{
    if (playerX >= 0 && playerY >= 0 && playerX < grid.size() && playerY < grid[0].size())
    {
        if (grid[playerX][playerY]->getType() == "portal")
        {
            Portal *portal = dynamic_cast<Portal *>(grid[playerX][playerY].get());
            if (portal)
            {
                portal->isActivated = true;
                showTLPConfirmation = true;
            }
        }
    }
}

void Maze::PlaceO(int ct)
{
    std::vector<std::pair<int, int>> EPS;
    for (int i = 0; i < grid.size(); ++i)
    {
        for (int j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j]->getType() == "empty")
            {
                EPS.emplace_back(i, j);
            }
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(EPS.begin(), EPS.end(), rng);

    ct = std::min(ct, static_cast<int>(EPS.size()));
    for (int i = 0; i < ct; ++i)
    {
        int x = EPS[i].first;
        int y = EPS[i].second;
        grid[x][y] = std::make_unique<Obstacle>(x, y);
    }
}

void Maze::moveGoal()
{
    for (int i = 0; i < grid.size(); ++i)
    {
        for (int j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j]->getType() == "goal")
            {
                MovableGoal *movableGoal = dynamic_cast<MovableGoal *>(grid[i][j].get());
                if (movableGoal)
                {
                    int oldX = movableGoal->getPosX();
                    int oldY = movableGoal->getPosY();
                    movableGoal->move(grid);
                    int newX = movableGoal->getPosX();
                    int newY = movableGoal->getPosY();
                    std::swap(grid[oldX][oldY], grid[newX][newY]);
                    return;
                }
            }
        }
    }
}