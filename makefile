CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -I/opt/homebrew/opt/sfml/include -fsanitize=address -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls
LDFLAGS = -L/opt/homebrew/opt/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system -fsanitize=address

OBJS = main.o block.o empty.o goal.o player.o portal.o key.o maze.o wall.o obstacle.o movable_goal.o
TARGET = brave_tour

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp maze.h block.h empty.h goal.h player.h portal.h key.h wall.h obstacle.h movable_goal.h
	$(CXX) $(CXXFLAGS) -c main.cpp

block.o: block.cpp block.h
	$(CXX) $(CXXFLAGS) -c block.cpp

empty.o: empty.cpp empty.h block.h
	$(CXX) $(CXXFLAGS) -c empty.cpp

goal.o: goal.cpp goal.h block.h
	$(CXX) $(CXXFLAGS) -c goal.cpp

player.o: player.cpp player.h block.h
	$(CXX) $(CXXFLAGS) -c player.cpp

portal.o: portal.cpp portal.h block.h
	$(CXX) $(CXXFLAGS) -c portal.cpp

key.o: key.cpp key.h block.h
	$(CXX) $(CXXFLAGS) -c key.cpp

maze.o: maze.cpp maze.h block.h key.h wall.h movable_goal.h
	$(CXX) $(CXXFLAGS) -c maze.cpp

obstacle.o: obstacle.cpp obstacle.h 
	$(CXX) $(CXXFLAGS) -c obstacle.cpp

wall.o: wall.cpp wall.h block.h
	$(CXX) $(CXXFLAGS) -c wall.cpp

movable_goal.o: movable_goal.cpp movable_goal.h goal.h
	$(CXX) $(CXXFLAGS) -c movable_goal.cpp

clean:
	rm -f $(OBJS) $(TARGET)