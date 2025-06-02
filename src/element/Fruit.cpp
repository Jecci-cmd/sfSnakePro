#include <SFML/Graphics.hpp>

#include "Fruit.h"
#include "Game.h"

using namespace sfSnake;

Fruit::Fruit(sf::Vector2f position, sf::Color color, int score)// 构造函数，创建一个水果对象。
    : score_(score)
{
    shape_.setRadius(Game::GlobalVideoMode.width / 128.0f);// 设置水果的半径
    setOriginMiddle(shape_);// 设置圆心为图形中心
    shape_.setPosition(position);// 设置位置
    shape_.setFillColor(color);// 设置颜色
}

void Fruit::render(sf::RenderWindow &window)
{
    window.draw(shape_);
}
