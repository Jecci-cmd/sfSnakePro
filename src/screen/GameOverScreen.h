#pragma once

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"
#include "screen/OptionScreen.h"

#include "element/Button.h"

namespace sfSnake
{
    class GameOverScreen : public Screen
    {
    public:
        GameOverScreen(std::size_t score);

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        sf::Text text_;
        
        std::vector<Button> button_;
        unsigned score_;

        OptionButton helpButton_;
        OptionButton aboutButton_;
        int scoreFromGame_;// 游戏结束时的分数
    };
}