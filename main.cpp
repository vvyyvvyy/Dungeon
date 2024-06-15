#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <tuple>
#include <chrono>
#include <queue>
#include "maze.h"
#include "block.h"
#include "empty.h"
#include "goal.h"
#include "player.h"
#include "movable_goal.h"
#include "portal.h"
#include "key.h"
#include <stdexcept>

#ifdef test_backpack
int Backpackct = 0;
#endif
bool GOD_MODE = false;
#define SLOT_DISTANCE 80

struct Node
{
    int x, y;
    float gCost, hCost, fCost;
    Node *parent;

    Node(int x, int y, float gCost, float hCost, Node *parent = nullptr)
        : x(x), y(y), gCost(gCost), hCost(hCost), fCost(gCost + hCost), parent(parent) {}

    bool operator>(const Node &other) const
    {
        return fCost > other.fCost;
    }
};

bool IOpen = false;
std::vector<sf::RectangleShape> ISLOT;
std::vector<std::string> ItNames = {"Torch", "Guiding Lantern", "Key Map", "TLP", "Unknown Treasure"};
std::vector<std::string> itemDescriptions = {"RV the whole maze", "A guiding light that shows the way to the destination in 30 sec", "Offers a fleeting glimpse of the route to the nearest key.", "TLP through walls within the restricted area.", "Unfind"};
int HVSlot = -1;
sf::Text IVTTitle;
sf::Text ItNameText;
sf::Text ITDescript;
bool TLPMode = false;
int TLPct = 3;
int initTLPX, initTLPY;
int TLPRange = 0;

std::vector<std::pair<int, int>> findPath(const std::vector<std::vector<std::unique_ptr<Block>>> &grid, int startX, int startY, int goalX, int goalY)
{
    std::vector<std::pair<int, int>> path;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<int, std::unordered_map<int, bool>> closedList;

    auto heuristic = [](int x1, int y1, int x2, int y2)
    {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    };

    openList.emplace(startX, startY, 0.0f, heuristic(startX, startY, goalX, goalY));

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        if (current.x == goalX && current.y == goalY)
        {
            Node *node = &current;
            while (node != nullptr)
            {
                path.emplace_back(node->x, node->y);
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            break;
        }

        closedList[current.x][current.y] = true;

        std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (const auto &[dx, dy] : directions)
        {
            int nx = current.x + dx;
            int ny = current.y + dy;

            if (nx >= 0 && nx < grid.size() && ny >= 0 && ny < grid[nx].size() && grid[nx][ny]->getType() != "wall" && !closedList[nx][ny])
            {
                float newGCost = current.gCost + 1.0f;
                float newHCost = heuristic(nx, ny, goalX, goalY);
                openList.emplace(nx, ny, newGCost, newHCost, new Node(current));
            }
        }
    }

    return path;
}
std::unique_ptr<Maze> loadMazeAPK(const std::string &filename, int keysND)
{
    auto maze = std::make_unique<Maze>(filename);
    auto &maze_grid = maze->getGR();

    std::vector<std::pair<int, int>> emptyPositions;
    for (int x = 0; x < maze_grid.size(); ++x)
    {
        for (int y = 0; y < maze_grid[x].size(); ++y)
        {
            if (maze_grid[x][y]->getType() == "empty")
            {
                emptyPositions.emplace_back(x, y);
            }
        }
    }

    std::shuffle(emptyPositions.begin(), emptyPositions.end(), std::default_random_engine(std::rand()));

    for (int i = 0; i < keysND && i < emptyPositions.size(); ++i)
    {
        int x = emptyPositions[i].first;
        int y = emptyPositions[i].second;
        maze_grid[x][y] = std::make_unique<Key>();
    }

    return maze;
}

int main()
{
    try
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        int curL = 1;
        int keysND = curL * 3;
        auto maze = loadMazeAPK("maze1.txt", keysND);

        player player;
        player.setMaze(maze->getGR());

        int WDW = 1500;
        int WDH = 900;
        int BSIZE = 30;

        sf::RenderWindow window(sf::VideoMode(WDW, WDH), "Dungeon Game", sf::Style::Titlebar | sf::Style::Close);

        sf::Font font_bold;
        if (!font_bold.loadFromFile("Silkscreen-Bold.ttf"))
        {
            std::cerr << "Failed to load font!" << std::endl;
            return -1;
        }

        sf::Font font_reg;
        if (!font_reg.loadFromFile("Silkscreen-Regular.ttf"))
        {
            std::cerr << "Failed to load font!" << std::endl;
            return -1;
        }

        sf::Text Congrats;
        Congrats.setFont(font_bold);
        Congrats.setString("Congratulations!");
        Congrats.setCharacterSize(50);
        Congrats.setFillColor(sf::Color::Yellow);

        sf::Text completed;
        completed.setFont(font_reg);
        completed.setString("You've completed the maze!");
        completed.setCharacterSize(30);
        completed.setFillColor(sf::Color::Yellow);
        sf::Text continueText;
        continueText.setFont(font_reg);
        continueText.setString("Press any key to continue");
        continueText.setCharacterSize(20);
        continueText.setFillColor(sf::Color::White);
        sf::Text pauseText;
        pauseText.setFont(font_bold);
        pauseText.setString("PAUSE");
        pauseText.setCharacterSize(100);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setOrigin(pauseText.getLocalBounds().width / 2, pauseText.getLocalBounds().height / 2);
        IVTTitle.setFont(font_bold);
        IVTTitle.setString("BACKPACK");
        IVTTitle.setCharacterSize(50);
        IVTTitle.setFillColor(sf::Color::White);
        IVTTitle.setPosition(WDW / 2.0f - 160, 150);
        ItNameText.setFont(font_reg);
        ItNameText.setCharacterSize(50);
        ItNameText.setFillColor(sf::Color::White);
        ItNameText.setPosition(WDW / 2.0f - 160, WDH - 300);
        ITDescript.setFont(font_reg);
        ITDescript.setCharacterSize(30);
        ITDescript.setFillColor(sf::Color::White);
        ITDescript.setPosition(WDW / 2.0f - 300, WDH - 150);

        for (int i = 0; i < 5; ++i)
        {
            sf::RectangleShape slot(sf::Vector2f(50, 50));
            slot.setPosition(WDW / 2.0f - 125 + i * 55, 300);
            ISLOT.push_back(slot);
        }

        bool mazeCompleted = false;
        bool waitingForKeyPress = false;
        bool isPaused = false;
        bool showPath = false;
        bool showKP = false;
        sf::View view = window.getView();
        float zoomLevel = 1.0f;

        sf::Clock clock;
        sf::Clock gameClock;
        sf::Time totalElapsedTime;
        float blinkInterval = 1.0f;

        sf::RectangleShape statusBar(sf::Vector2f(300, 170));
        statusBar.setFillColor(sf::Color::Black);

        sf::Text statusText;
        statusText.setFont(font_reg);
        statusText.setCharacterSize(20);
        statusText.setFillColor(sf::Color::White);

        sf::Text GT_TX;
        GT_TX.setFont(font_reg);
        GT_TX.setCharacterSize(30);
        GT_TX.setFillColor(sf::Color::White);

        sf::Text G_MText;
        G_MText.setFont(font_bold);
        G_MText.setString("GOD MODE");
        G_MText.setCharacterSize(60);
        G_MText.setFillColor(sf::Color::Yellow);
        G_MText.setOutlineColor(sf::Color::White);
        G_MText.setOutlineThickness(2);

        std::unordered_map<std::tuple<int, int>, TileInfo, tuple_hash> EXP_A;
        std::vector<std::pair<int, int>> path;
        std::vector<std::pair<int, int>> keyPath;

        auto updateTextPositions = [&]()
        {
            sf::Vector2f viewCenter = view.getCenter();

            Congrats.setCharacterSize(static_cast<unsigned int>(50 * zoomLevel));
            completed.setCharacterSize(static_cast<unsigned int>(30 * zoomLevel));
            continueText.setCharacterSize(static_cast<unsigned int>(20 * zoomLevel));
            pauseText.setCharacterSize(static_cast<unsigned int>(100 * zoomLevel));

            sf::FloatRect textRect = Congrats.getLocalBounds();
            Congrats.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            Congrats.setPosition(viewCenter.x, viewCenter.y - 50 * zoomLevel);

            sf::FloatRect completedRect = completed.getLocalBounds();
            completed.setOrigin(completedRect.left + completedRect.width / 2.0f, completedRect.top + completedRect.height / 2.0f);
            completed.setPosition(viewCenter.x, viewCenter.y + 30 * zoomLevel);

            sf::FloatRect continueTextRect = continueText.getLocalBounds();
            continueText.setOrigin(continueTextRect.left + continueTextRect.width / 2.0f, continueTextRect.top + continueTextRect.height / 2.0f);
            continueText.setPosition(viewCenter.x, viewCenter.y + 100 * zoomLevel);

            sf::FloatRect pauseTextRect = pauseText.getLocalBounds();
            pauseText.setOrigin(pauseTextRect.left + pauseTextRect.width / 2.0f, pauseTextRect.top + pauseTextRect.height / 2.0f);
            pauseText.setPosition(viewCenter.x, viewCenter.y);

            sf::FloatRect gameTimeRect = GT_TX.getLocalBounds();

            sf::Vector2u windowSize = window.getSize();
            G_MText.setOrigin(gameTimeRect.left + gameTimeRect.width / 2.0f, gameTimeRect.top + gameTimeRect.height / 2.0f);
            G_MText.setPosition(windowSize.x / 2.0f - 130, gameTimeRect.height + 60);
        };

        auto updateSBP = [&]()
        {
            sf::Vector2u windowSize = window.getSize();

            statusBar.setPosition(windowSize.x - statusBar.getSize().x - 10, windowSize.y - statusBar.getSize().y - 10);
            statusText.setPosition(statusBar.getPosition().x + 10, statusBar.getPosition().y + 10);

            sf::FloatRect GT_TXRect = GT_TX.getLocalBounds();
            GT_TX.setOrigin(GT_TXRect.left + GT_TXRect.width / 2.0f, GT_TXRect.top + GT_TXRect.height / 2.0f);
            GT_TX.setOutlineColor(sf::Color::Black);
            GT_TX.setOutlineThickness(2);
            GT_TX.setPosition(windowSize.x / 2.0f - 73, GT_TXRect.height / 2.0f + 10);
        };

        updateSBP();
        maze->PlaceO(curL * 3);

        while (window.isOpen())
        {
#ifdef player_pos
            std::cout << "player Position:(" << player.getPosX() << "," << player.getPosY() << ")" << std::endl;
#endif
            sf::Event event;
            bool playerMoved = false;
            bool playerPickedUpKey = false;

            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    updateSBP();
                }

                maze->display(window, player, EXP_A);
                if (maze->GStele())
                {
                    maze->drawTLPConfirmation(window);
                }

#ifdef test_mouse
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                std::cout << "(" << mousePos.x << "," << mousePos.y << ")" << std::endl;
#endif

                if (IOpen)
                {

                    if (event.type == sf::Event::Closed)
                        window.close();

                    if (event.type == sf::Event::MouseMoved)
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        HVSlot = -1;
                        for (int i = 0; i < ISLOT.size(); ++i)
                        {
                            if (ISLOT[i].getGlobalBounds().contains(mousePos.x, mousePos.y))
                            {
                                HVSlot = i;
#ifdef test_mouse
                                std::cout << "slot " << HVSlot << std::endl;
#endif
                                break;
                            }
                        }
                    }

                    if (event.type == sf::Event::KeyPressed)
                    {
                        if (event.key.code == sf::Keyboard::B)
                        {
                            IOpen = !IOpen;
                        }

                        if (event.key.code == sf::Keyboard::Escape)
                        {
                            window.close();
                            break;
                        }
                    }

                    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (HVSlot != -1)
                        {
                            std::cout << "\"" << ItNames[HVSlot] << "\" " << " selected" << std::endl;
                        }
                    }
                    continue;
                }

                if (event.type == sf::Event::KeyPressed)
                {

                    bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

                    if (event.key.code == sf::Keyboard::P && !IOpen)
                    {
                        isPaused = !isPaused;
                        if (isPaused)
                        {
                            totalElapsedTime += gameClock.getElapsedTime();
                            gameClock.restart();
                        }
                        else
                        {
                            gameClock.restart();
                        }
                    }

                    if (event.key.code == sf::Keyboard::B)
                    {
                        if (IOpen)
                            IOpen = !IOpen;
                        else if (!IOpen && !isPaused)
                            IOpen = !IOpen;
                    }

                    if (event.key.code == sf::Keyboard::Num6)
                    {
                        if (GOD_MODE)
                            GOD_MODE = false;
                        else
                            GOD_MODE = true;
                    }

                    if (event.key.code == sf::Keyboard::Space)
                    {
                        player.attack(EXP_A);
                    }

                    if (isPaused)
                    {
                        continue;
                    }

                    if (mazeCompleted && waitingForKeyPress)
                    {
                        curL++;
                        if (curL > 6)
                        {
                            window.close();
                            break;
                        }

                        keysND = curL * 3;

                        std::string nextMaze = "maze" + std::to_string(curL) + ".txt";

#ifdef LoadMaze_debug
                        std::cout << "Loading layer " << curL << std::endl;
                        std::cout << "Loading maze from file: " << nextMaze << std::endl;
#endif
                        maze = loadMazeAPK(nextMaze, keysND);

                        maze->PlaceO(curL * 5);

                        player.setMaze(maze->getGR());
                        player.resetKted();
                        player.setP(1, 1);

                        EXP_A.clear();
                        mazeCompleted = false;
                        waitingForKeyPress = false;
                        path.clear();
                        continue;
                    }

                    char input = '\0';
                    if (event.key.code == sf::Keyboard::W)
                        input = 'W';
                    if (event.key.code == sf::Keyboard::A)
                        input = 'A';
                    if (event.key.code == sf::Keyboard::S)
                        input = 'S';
                    if (event.key.code == sf::Keyboard::D)
                        input = 'D';
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        window.close();
                        break;
                    }
                    if (event.key.code == sf::Keyboard::Equal)
                    {
                        zoomLevel *= 0.9f;
                        view.zoom(0.9f);
                        view.setCenter(player.getPosY() * BSIZE + BSIZE / 2.0f, player.getPosX() * BSIZE + BSIZE / 2.0f);
                    }
                    if (event.key.code == sf::Keyboard::Hyphen)
                    {
                        zoomLevel *= 1.1f;
                        view.zoom(1.1f);
                        view.setCenter(player.getPosY() * BSIZE + BSIZE / 2.0f, player.getPosX() * BSIZE + BSIZE / 2.0f);
                    }

                    if (event.key.code == sf::Keyboard::L && GOD_MODE)
                    {
                        for (int x = 0; x < maze->getGR().size(); ++x)
                        {
                            for (int y = 0; y < maze->getGR()[x].size(); ++y)
                            {
                                sf::Color color;
                                std::string type = maze->getGR()[x][y]->getType();
                                if (type == "wall")
                                    color = sf::Color::Black;
                                else if (type == "empty")
                                    color = sf::Color::White;
                                else if (type == "goal")
                                    color = sf::Color::Green;
                                else if (type == "key")
                                    color = sf::Color::Yellow;

                                EXP_A[std::make_tuple(x, y)] = {color, std::chrono::steady_clock::now()};
                            }
                        }
                    }

                    if (event.key.code == sf::Keyboard::O && GOD_MODE)
                    {
                        if (showPath)
                        {
                            path.clear();
                        }
                        else
                        {
                            for (int x = 0; x < maze->getGR().size(); ++x)
                            {
                                for (int y = 0; y < maze->getGR()[x].size(); ++y)
                                {
                                    if (maze->getGR()[x][y]->getType() == "goal")
                                    {
                                        path = findPath(maze->getGR(), player.getPosX(), player.getPosY(), x, y);
                                        break;
                                    }
                                }
                            }
                        }
                        showPath = !showPath;
                    }

                    if (event.key.code == sf::Keyboard::K && GOD_MODE)
                    {
                        if (showKP)
                        {
                            keyPath.clear();
                        }
                        else
                        {
                            int keyX = -1, keyY = -1;
                            float minDist = std::numeric_limits<float>::max();
                            for (int x = 0; x < maze->getGR().size(); ++x)
                            {
                                for (int y = 0; y < maze->getGR()[x].size(); ++y)
                                {
                                    if (maze->getGR()[x][y]->getType() == "key")
                                    {
                                        float dist = std::abs(x - player.getPosX()) + std::abs(y - player.getPosY());
                                        if (dist < minDist)
                                        {
                                            minDist = dist;
                                            keyX = x;
                                            keyY = y;
                                        }
                                    }
                                }
                            }
                            if (keyX != -1 && keyY != -1)
                            {
                                keyPath = findPath(maze->getGR(), player.getPosX(), player.getPosY(), keyX, keyY);
                            }
                        }
                        showKP = !showKP;
                    }

                    if (GOD_MODE)
                    {
                        if (event.key.code == sf::Keyboard::T)
                        {
                            if (TLPMode)
                            {
                                TLPMode = false;
                            }
                            else
                            {
                                TLPMode = true;

                                initTLPX = player.getPosX();
                                initTLPY = player.getPosY();
                                TLPRange = 2 * curL + 1;
                            }
                        }
                    }
                    else
                    {
                        if (event.key.code == sf::Keyboard::T && TLPct > 0)
                        {
                            if (TLPMode)
                            {
                                TLPMode = false;
                                TLPct--;
                            }
                            else
                            {
                                TLPMode = true;

                                initTLPX = player.getPosX();
                                initTLPY = player.getPosY();
                                TLPRange = 2 * curL + 1;
                            }
                        }
                    }

                    if (TLPMode)
                    {
                        int newX = player.getPosX();
                        int newY = player.getPosY();

                        if (event.key.code == sf::Keyboard::W)
                            newX--;
                        if (event.key.code == sf::Keyboard::A)
                            newY--;
                        if (event.key.code == sf::Keyboard::S)
                            newX++;
                        if (event.key.code == sf::Keyboard::D)
                            newY++;

                        if (newX >= initTLPX - TLPRange / 2 && newX <= initTLPX + TLPRange / 2 &&
                            newY >= initTLPY - TLPRange / 2 && newY <= initTLPY + TLPRange / 2 &&
                            newX >= 0 && newX < maze->getGR().size() &&
                            newY >= 0 && newY < maze->getGR()[newX].size())
                        {

                            if (maze->getGR()[newX][newY]->getType() == "empty")
                            {
                                player.setP(newX, newY);
                            }
                            else if (maze->getGR()[newX][newY]->getType() == "wall")
                            {

                                int nx = newX, ny = newY;
                                while (nx >= initTLPX - TLPRange / 2 && nx <= initTLPX + TLPRange / 2 &&
                                       ny >= initTLPY - TLPRange / 2 && ny <= initTLPY + TLPRange / 2 &&
                                       nx >= 0 && nx < maze->getGR().size() &&
                                       ny >= 0 && ny < maze->getGR()[nx].size() &&
                                       maze->getGR()[nx][ny]->getType() == "wall")
                                {
                                    if (event.key.code == sf::Keyboard::W)
                                        nx--;
                                    if (event.key.code == sf::Keyboard::A)
                                        ny--;
                                    if (event.key.code == sf::Keyboard::S)
                                        nx++;
                                    if (event.key.code == sf::Keyboard::D)
                                        ny++;
                                }
                                if (nx >= 0 && nx < maze->getGR().size() &&
                                    ny >= 0 && ny < maze->getGR()[nx].size() &&
                                    maze->getGR()[nx][ny]->getType() == "empty")
                                {
                                    player.setP(nx, ny);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (input != '\0' && !mazeCompleted)
                        {
                            if (shiftPressed)
                            {
                                int newX = player.getPosX();
                                int newY = player.getPosY();

                                if (input == 'W')
                                {
                                    while (newX > 0 && maze->getGR()[newX - 1][newY]->getType() != "wall")
                                        newX--;
                                }
                                else if (input == 'A')
                                {
                                    while (newY > 0 && maze->getGR()[newX][newY - 1]->getType() != "wall")
                                        newY--;
                                }
                                else if (input == 'S')
                                {
                                    while (newX < maze->getGR().size() - 1 && maze->getGR()[newX + 1][newY]->getType() != "wall")
                                        newX++;
                                }
                                else if (input == 'D')
                                {
                                    while (newY < maze->getGR()[newX].size() - 1 && maze->getGR()[newX][newY + 1]->getType() != "wall")
                                        newY++;
                                }

                                player.setP(newX, newY);
                            }
                            else
                            {
                                player.move(input);
                            }
                            playerMoved = true;
                        }
                    }

                    view.setCenter(player.getPosY() * BSIZE + BSIZE / 2.0f, player.getPosX() * BSIZE + BSIZE / 2.0f);
                    window.setView(view);

                    updateTextPositions();
                }
            }

            window.clear();

            if (isPaused && !IOpen)
            {
                window.clear(sf::Color::Black);
                window.draw(pauseText);
            }

            else if (mazeCompleted)
            {
                window.clear(sf::Color::Black);
                window.draw(Congrats);
                switch (curL)
                {
                case 1:
                    completed.setString("You've completed Layer 1!");
                    break;
                case 2:
                    completed.setString("You've completed Layer 2!");
                    break;
                case 3:
                    completed.setString("You've completed Layer 3!");
                    break;
                case 4:
                    completed.setString("You've completed Layer 4!");
                    break;
                case 5:
                    completed.setString("You've completed Layer 5!");
                    break;
                default:
                    completed.setString("You've completed all mazes!");
                    break;
                }
                window.draw(completed);

                float time = clock.getElapsedTime().asSeconds();
                float phase = (std::sin(time * 3.14159f / blinkInterval) + 1.0f) / 2.0f;
                sf::Uint8 colorValue = static_cast<sf::Uint8>(255 * phase);
                continueText.setFillColor(sf::Color(colorValue, colorValue, colorValue));

                window.draw(continueText);

                waitingForKeyPress = true;
            }
            else
            {
                maze->display(window, player, EXP_A);

                if (showPath)
                {
                    for (const auto &pos : path)
                    {
                        sf::RectangleShape pathRect(sf::Vector2f(BSIZE, BSIZE));
                        pathRect.setPosition(pos.second * BSIZE, pos.first * BSIZE);
                        pathRect.setFillColor(sf::Color(0, 0, 255, 128));
                        window.draw(pathRect);
                    }
                }

                if (showKP)
                {
                    for (const auto &pos : keyPath)
                    {
                        sf::RectangleShape pathRect(sf::Vector2f(BSIZE, BSIZE));
                        pathRect.setPosition(pos.second * BSIZE, pos.first * BSIZE);
                        pathRect.setFillColor(sf::Color(255, 255, 0, 128));
                        window.draw(pathRect);
                    }
                }

                if (TLPMode)
                {

                    for (int dx = -TLPRange / 2; dx <= TLPRange / 2; ++dx)
                    {
                        for (int dy = -TLPRange / 2; dy <= TLPRange / 2; ++dy)
                        {
                            int nx = initTLPX + dx;
                            int ny = initTLPY + dy;
                            if (nx >= 0 && nx < maze->getGR().size() && ny >= 0 && ny < maze->getGR()[nx].size())
                            {
                                sf::RectangleShape redRect(sf::Vector2f(BSIZE, BSIZE));
                                redRect.setPosition(ny * BSIZE, nx * BSIZE);
                                redRect.setFillColor(sf::Color(255, 0, 0, 128));
                                window.draw(redRect);
                            }
                        }
                    }
                }

                if (playerMoved || playerPickedUpKey)
                {
                    int playerX = player.getPosX();
                    int playerY = player.getPosY();

                    if (playerX >= 0 && playerY >= 0 && playerX < maze->getGR().size() && playerY < maze->getGR()[0].size())
                    {
                        if (maze->getGR()[playerX][playerY]->getType() == "portal")
                        {

#ifdef portal_confirmation_debug
                            std::cout << "portal detected";
#endif

                            maze->setShowTLPConfirmation(true);
                        }
                        else
                        {
                            maze->setShowTLPConfirmation(false);
                        }
                    }

                    maze->moveGoal();

                    if (!playerPickedUpKey)
                    {
                        keyPath.clear();
                        showKP = false;
                    }

                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        for (int dy = -1; dy <= 1; ++dy)
                        {
                            int nx = playerX + dx;
                            int ny = playerY + dy;
                            if (nx >= 0 && nx < maze->getGR().size() && ny >= 0 && ny < maze->getGR()[nx].size())
                            {
                                sf::Color color;
                                std::string type = maze->getGR()[nx][ny]->getType();
                                if (type == "wall")
                                    color = sf::Color::Black;
                                else if (type == "empty")
                                    color = sf::Color::White;
                                else if (type == "goal")
                                    color = sf::Color::Green;
                                else if (type == "key")
                                    color = sf::Color::Yellow;

                                EXP_A[std::make_tuple(nx, ny)] = {color, std::chrono::steady_clock::now()};
                            }
                        }
                    }

                    playerMoved = false;
                    playerPickedUpKey = false;
                }

                if (!isPaused)
                {
                    for (auto &[pos, info] : EXP_A)
                    {
                        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - info.lastSeen).count();
                        if (elapsed < 60)
                        {
                            float factor = 1.0f - static_cast<float>(elapsed) / 60.0f;
                            info.color.a = static_cast<sf::Uint8>(255 * factor);
                        }
                        else
                        {
                            info.color = sf::Color::Black;
                        }
                    }
                }

                std::string statusString = "HP: 100/100\nATK: 10\nLEVEL: " + std::to_string(curL) +
                                           "\nKeys: " + std::to_string(player.GetK()) + "/" + std::to_string(keysND) +
                                           "\nLocation: (" + std::to_string(player.getPosX()) + ", " + std::to_string(player.getPosY()) + ")" +
                                           "\nTLPs: " + std::to_string(TLPct);
                statusText.setString(statusString);

                if (!mazeCompleted && player.GetK() >= (GOD_MODE ? 0 : keysND))
                {
                    int playerX = player.getPosX();
                    int playerY = player.getPosY();
                    if (maze->getGR()[playerX][playerY]->getType() == "goal")
                    {
                        MovableGoal *movableGoal = dynamic_cast<MovableGoal *>(maze->getGR()[playerX][playerY].get());
                        if (movableGoal || maze->getGR()[playerX][playerY]->getType() == "goal")
                        {
                            mazeCompleted = true;
                        }
                    }
                }

                window.setView(window.getDefaultView());
                window.draw(statusBar);
                window.draw(statusText);
                window.setView(view);
            }

            sf::Time gameTime = totalElapsedTime;
            if (!isPaused)
            {
                gameTime += gameClock.getElapsedTime();
            }

            if (isPaused)
            {
                GT_TX.setString("Time: Paused");
            }
            else
            {
                GT_TX.setString("Time: " + std::to_string(static_cast<int>(gameTime.asSeconds())));
            }

            window.setView(window.getDefaultView());
            window.draw(GT_TX);
            if (GOD_MODE)
            {
                window.draw(G_MText);
            }

            if (IOpen)
            {
                sf::RectangleShape inventoryBackground(sf::Vector2f(WDW, WDH));
                inventoryBackground.setFillColor(sf::Color(0, 0, 0, 220));
                window.draw(inventoryBackground);
#ifdef test_backpack
                std::cout << "backpack_opened" << Backpackct << std::endl;
                Backpackct++;
#endif

                const float animationSpeed = 30.0f;
                float totalWidth = ISLOT.size() * 50.0f + (ISLOT.size() - 1) * 5.0f;
                float startX = WDW / 2.0f - totalWidth / 2.0f;

                float deltaTime = clock.restart().asSeconds();
                int SlotDistance = SLOT_DISTANCE;

                window.draw(IVTTitle);
                for (int i = 0; i < ISLOT.size(); ++i)
                {
                    sf::Vector2f slotSize(60, 60);
                    sf::Vector2f slotPosition(WDW / 2.0f - 140 + i * SlotDistance, 300);

                    if (HVSlot == i)
                    {
                        ISLOT[i].setFillColor(sf::Color::White);
                        slotSize = sf::Vector2f(80, 80);
#ifdef test_mouse
                        std::cout << "Slot " << i << " is HV and Enlarging ." << std::endl;
#endif
                    }
                    else
                    {
                        ISLOT[i].setFillColor(sf::Color(128, 128, 128));
                        slotSize = sf::Vector2f(50, 50);
                    }

                    ISLOT[i].setOrigin(25.0f, 25.0f);

                    sf::Vector2f targetScale(slotSize.x / 50.0f, slotSize.y / 50.0f);

                    sf::Vector2f currentScale = ISLOT[i].getScale();

                    sf::Vector2f newScale = currentScale + (targetScale - currentScale) * deltaTime * animationSpeed;

                    ISLOT[i].setScale(newScale);

                    ISLOT[i].setPosition(slotPosition);

                    float offset = (ISLOT[i].getGlobalBounds().width - 50.0f) / 2.0f;
                    startX += offset;

                    window.draw(ISLOT[i]);

                    startX += 50.0f + offset + 5.0f;
                }

                if (HVSlot != -1)
                {
                    ItNameText.setString(ItNames[HVSlot]);
                    ITDescript.setString(itemDescriptions[HVSlot]);
                }
                else
                {
                    ItNameText.setString("");
                    ITDescript.setString("");
                }
                window.draw(ItNameText);
                window.draw(ITDescript);
            }

            window.setView(view);
            window.display();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
