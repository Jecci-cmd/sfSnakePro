#include <fstream>
#include <iostream>
#include "RankScreen.h"
#include "Game.h"
#include "MenuScreen.h"
#include "element/Button.h"

using namespace sfSnake;

int RankScreen::highestCoins_ = 0;

RankScreen::RankScreen() :backButton_() {
    font_ = Game::GlobalFont;
    backButton_.update("assets/image/return.png");
    backButton_.setPosition(Game::GlobalVideoMode.width / 15.0f, Game::GlobalVideoMode.width / 15.0f);

    titleText_.setFont(font_);
    titleText_.setString(L"成就记录");
    titleText_.setCharacterSize(60);
    titleText_.setFillColor(sf::Color::Yellow);
    titleText_.setPosition(Game::GlobalVideoMode.width * 0.38f, 80.f);

    highScoreText_.setFont(font_);
    highScoreText_.setCharacterSize(60);
    highScoreText_.setFillColor(sf::Color(255, 182, 193));  // 浅粉
    highScoreText_.setPosition(Game::GlobalVideoMode.width * 0.3f,
                           Game::GlobalVideoMode.height * 0.4f);

    if (backgroundTexture_.loadFromFile("assets/image/menubg.png")) {
        backgroundSprite_.setTexture(backgroundTexture_);
    }

    LoadData();

    highScoreText_.setString(L"最高金币数: " + std::to_wstring(highestCoins_));
}

void RankScreen::handleInput(sf::RenderWindow &window) {

    auto mousePosition = sf::Mouse::getPosition(window);

    backButton_.focused(false);
    if (backButton_.contain(mousePosition))
    {
        backButton_.focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::MainScreen = Game::TmpScreen;
            Game::TmpScreen = nullptr;
            return;
        }
    }
}

void RankScreen::update(sf::Time) {}

void RankScreen::render(sf::RenderWindow &window) {
    window.draw(backgroundSprite_); // 绘制背景
    window.setView(window.getDefaultView());
    backButton_.render(window);
    window.draw(titleText_);
    window.draw(highScoreText_);
    window.draw(backHintText_);
}

void RankScreen::LoadData() {
    std::ifstream file("rank.txt");
    if (file.is_open()) {
        file >> highestCoins_;
    }
}

void RankScreen::SaveData(int latestCoins) {
    LoadData();  
    if (latestCoins > highestCoins_) {
        highestCoins_ = latestCoins;
        std::ofstream file("rank.txt");
        if (file.is_open()) {
            file << highestCoins_;
        }
    }
}
