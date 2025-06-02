#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"

#include "element/Button.h"
#include "element/OptionButton.h"


namespace sfSnake
{
    class MenuScreen : public Screen
    {
    public:
        MenuScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        std::vector<Button> button_;
        OptionButton helpButton_;
        OptionButton aboutButton_;
        
        // 背景和金币显示
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        sf::Text coinsText_;  // 金币显示文本
        void UpdateCoinsDisplay();  // 更新金币显示
    };
}