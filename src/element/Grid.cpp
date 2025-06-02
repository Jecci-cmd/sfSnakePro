#include "element/Grid.h"
#include "Game.h"

using namespace sfSnake;

Grid::Grid()
    : gridVline(sf::Vector2f(Game::GlobalVideoMode.width / 500.0f, Game::mapHeight)),
      gridHline(sf::Vector2f(Game::mapWidth, Game::GlobalVideoMode.width / 500.0f))
{
}

void Grid::render(sf::RenderWindow &window)
{
    gridVline.setFillColor(Game::Color::Grid[Game::GridColor]);
    gridHline.setFillColor(Game::Color::Grid[Game::GridColor]);

    static float gridSpace = Game::GlobalVideoMode.width / 100.0f;
    for (float i = 0.f; i < Game::mapWidth; i += gridSpace)
    {
        gridVline.setPosition(i, 0);
        window.draw(gridVline);
    }

    for (float i = 0.f; i < Game::mapHeight; i += gridSpace)
    {
        gridHline.setPosition(0, i);
        window.draw(gridHline);
    }
}
