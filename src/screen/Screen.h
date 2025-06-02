#pragma once

#include <SFML/Graphics.hpp>

class Screen
{
public:
	virtual void handleInput(sf::RenderWindow &window) = 0;
	virtual void update(sf::Time delta) = 0;
	virtual void render(sf::RenderWindow &window) = 0;
	sf::Texture backgroundTexture_;//图像原始数据
	sf::Sprite backgroundSprite_;//精灵
};