#pragma once

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "Game.h"

#include "screen/Screen.h"

#include "element/Button.h"
#include "element/OptionButton.h"
namespace sfSnake
{
    class PauseScreen : public Screen
    {
    public:
        PauseScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        std::vector<Button> button_;
        Button returnButton_;
        OptionButton helpButton_;
        OptionButton aboutButton_;
    };
}