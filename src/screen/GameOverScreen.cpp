#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

#include "Game.h"

#include "screen/GameScreen.h"
#include "screen/GameOverScreen.h"
#include "screen/HelpScreen.h"
#include "screen/AboutScreen.h"
#include "screen/MenuScreen.h"
#include "screen/RankScreen.h"
#include "screen/StoreScreen.h"

using namespace sfSnake;

GameOverScreen::GameOverScreen(std::size_t score)
    : button_(3), score_(score)
{
    StoreScreen::AddCoins(score);
    RankScreen::SaveData(score);
    text_.setFont(Game::GlobalFont);
    text_.setString(sf::String(L"本局获得金币数：") + std::to_wstring(score));
    text_.setFillColor(Game::Color::Yellow);
    text_.setCharacterSize(Game::GlobalVideoMode.width / 15);
    setOriginMiddle(text_);
    text_.setPosition(Game::GlobalVideoMode.width / 2.0f, Game::GlobalVideoMode.height / 4.0f);

    button_[0].update("assets/image/optionUI.png");
    button_[1].update("assets/image/return.png", 170.f);
    button_[2].update("assets/image/exitUI.png");

    button_[0].setPosition(Game::GlobalVideoMode.width / 3.0, Game::GlobalVideoMode.height / 5.0 * 3.0);
    button_[1].setPosition(Game::GlobalVideoMode.width / 15.0f, Game::GlobalVideoMode.width / 15.0f);
    button_[2].setPosition(Game::GlobalVideoMode.width / 3.0 * 2.0, Game::GlobalVideoMode.height / 5.0 * 3.0);

    helpButton_.settings(
        L"帮助",
        Game::GlobalFont,
        Game::GlobalVideoMode.width / 20.0,
        sf::Color(255,182,193),
        sf::Vector2f(Game::GlobalVideoMode.width / 5.0f * 2.0f, Game::GlobalVideoMode.height / 5.0f * 4.0f));

    aboutButton_.settings(
        L"关于",
        Game::GlobalFont,
        Game::GlobalVideoMode.width / 20.0,
        sf::Color(255,182,193),
        sf::Vector2f(Game::GlobalVideoMode.width / 5.0f * 3.0f, Game::GlobalVideoMode.height / 5.0f * 4.0f));

    // 背景
    if(!backgroundTexture_.loadFromFile("assets/image/menubg.png"))
    {
        std::cerr << "Failed to load background texture" << std::endl;
    }
    backgroundSprite_.setTexture(backgroundTexture_);
}

void GameOverScreen::handleInput(sf::RenderWindow &window)
{
    static sf::Vector2i mousePosition;
    mousePosition = sf::Mouse::getPosition(window);

    for (auto &i : button_)
        i.focused(false);
    button_[1].focused(false);
    helpButton_.clear();
    aboutButton_.clear();

    if (button_[0].contain(mousePosition))
    {
        button_[0].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<OptionScreen>();
            return;
        }
    }

    if (button_[1].contain(mousePosition))
    {
        button_[1].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::MainScreen = std::make_shared<MenuScreen>();;
            Game::TmpScreen = nullptr;
            return;
        }
    }

    if (button_[2].contain(mousePosition))
    {
        button_[2].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            window.close();
            return;
        }
    }
    

    if (helpButton_.contains(mousePosition))
    {
        helpButton_.focused();
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            helpButton_.seleted();
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<HelpScreen>();
            return;
        }
    }

    if (aboutButton_.contains(mousePosition))
    {
        aboutButton_.focused();
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            aboutButton_.seleted();
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<AboutScreen>();
            return;
        }
    }
}

void GameOverScreen::update(sf::Time delta)
{
    Game::GlobalTitle.update(delta);
    Game::mouseButtonCDtime += delta;

}

void GameOverScreen::render(sf::RenderWindow &window)
{
    window.draw(backgroundSprite_);
    window.setView(window.getDefaultView());
    window.draw(text_);
    button_[0].render(window);
    button_[1].render(window);
    button_[2].render(window);
    helpButton_.render(window);
    aboutButton_.render(window);
}