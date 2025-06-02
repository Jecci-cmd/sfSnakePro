#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Game.h"

#include "screen/GameScreen.h"
#include "screen/AboutScreen.h"
#include "screen/OptionScreen.h"

using namespace sfSnake;

AboutScreen::AboutScreen()
{
    text_.setFont(Game::GlobalFont);
    text_.setString(
        sf::String(L"(魔改版)oop作业\n\n") +
        sf::String(L"原作者：精益君(JingYiJun3104@outlook.com)\n\n") +
        sf::String(L"李佳羲(jiaxili24@m.fudan.edu.cn)\n\n") );
    text_.setCharacterSize(Game::GlobalVideoMode.width / 35.0f);
    text_.setFillColor(sf::Color(255,182,193));
    setOriginMiddle(text_);
    text_.setPosition(Game::GlobalVideoMode.width / 2.0, Game::GlobalVideoMode.height / 5.0 * 3.0); 

    if (!returnButton_.update("assets/image/return.png", 170.f)) {
    std::cerr << "can't load assets/image/return.png" << std::endl;
    }

    returnButton_.setPosition(Game::GlobalVideoMode.width / 15.0f, Game::GlobalVideoMode.width / 15.0f);
    if(!backgroundTexture_.loadFromFile("assets/image/menubg.png"))
    {
        std::cerr << "Failed to load background texture" << std::endl;
    }
    backgroundSprite_.setTexture(backgroundTexture_);
    
}

void AboutScreen::handleInput(sf::RenderWindow &window) 
{
    auto mousePosition = sf::Mouse::getPosition(window);

    returnButton_.focused(false);// 先默认按钮不高亮，如果鼠标在按钮范围内则高亮
    if (returnButton_.contain(mousePosition))
    {
        returnButton_.focused(true);
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

void AboutScreen::update(sf::Time delta) // 更新全局标题动画或状态，delta是两帧之间的时差
{
    Game::GlobalTitle.update(delta);
}

void AboutScreen::render(sf::RenderWindow &window)
{
    window.draw(backgroundSprite_);
    window.setView(window.getDefaultView()); 
    Game::GlobalTitle.render(window);
    window.draw(text_);
    returnButton_.render(window);
}