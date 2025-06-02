#include "element/Button.h"
#include "Game.h"

using namespace sfSnake;

Button::Button()
    : focused_{false}
{
}

bool Button::update(std::string filename, float targetWidth)
{
    if (!texture_.loadFromFile(filename))
        return false;
    texture_.setSmooth(true);
    sprite_.setTexture(texture_);
    sf::FloatRect bounds = sprite_.getLocalBounds();
    sprite_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    float scale = targetWidth / bounds.width; 
    sprite_.setScale(scale, scale);
    return true;
}


void Button::setPosition(float x, float y)
{
    sprite_.setPosition(x, y);
}

void Button::focused(bool status)
{
    focused_ = status;
    if (status)
        sprite_.setColor(sf::Color(255,182,193));
    else
        sprite_.setColor(sf::Color::White);
}

bool Button::focused() const
{
    return focused_;
}

void Button::render(sf::RenderWindow &window) const
{
    window.draw(sprite_);
}

