#include <iostream>
#include <windows.h>

#include "Game.h"

using namespace sfSnake;

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    Game game;
    std::cout << "Program started!" << std::endl;

    game.run();
    sf::RenderWindow window(sf::VideoMode(800, 600), "My Game");


    return 0;
}