#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include "element/Button.h"

namespace sfSnake {

class RankScreen : public Screen {
public:
    RankScreen();

    void handleInput(sf::RenderWindow &window) override;
    void update(sf::Time delta) override;
    void render(sf::RenderWindow &window) override;

    static void LoadData();
    static void SaveData(int latestCoins);

private:
    Button backButton_;
    sf::Font font_;
    sf::Text titleText_;
    sf::Text highScoreText_;
    sf::Text backHintText_;
    sf::Texture backgroundTexture_;
    sf::Sprite backgroundSprite_;

    static int highestCoins_;
};

} 
