#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Game.h"

#include "screen/GameScreen.h"
#include "screen/MenuScreen.h"
#include "screen/OptionScreen.h"
#include "screen/HelpScreen.h"
#include "screen/AboutScreen.h"
#include "screen/StoreScreen.h"
#include "screen/RankScreen.h"

using namespace sfSnake;

MenuScreen::MenuScreen()
    : button_(5)
{
    Game::GlobalFont.loadFromFile("assets/fonts/SourceHanSansSC-Bold.otf");

    button_[0].update("assets/image/optionUI.png");
    button_[1].update("assets/image/startUI.png",300.f);
    button_[2].update("assets/image/exitUI.png");
    button_[3].update("assets/image/storeUI.png");
    button_[4].update("assets/image/rankUI.png");

    // 按钮0：左上角
    button_[0].setPosition(Game::GlobalVideoMode.width * 0.15f,
                           Game::GlobalVideoMode.height * 0.4f);

    // 按钮1：中心
    button_[1].setPosition(Game::GlobalVideoMode.width * 0.5f,
                           Game::GlobalVideoMode.height * 0.5f);

    // 按钮2：左下
    button_[2].setPosition(Game::GlobalVideoMode.width * 0.15f,
                           Game::GlobalVideoMode.height * 0.7f);
    button_[3].setPosition(Game::GlobalVideoMode.width * 0.85f,
                             Game::GlobalVideoMode.height * 0.4f);
    button_[4].setPosition(Game::GlobalVideoMode.width * 0.85f,
                             Game::GlobalVideoMode.height * 0.7f);

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

    //背景图片
    if(!backgroundTexture_.loadFromFile("assets/image/menubg.png"))
    {
        std::cerr << "Failed to load background texture" << std::endl;
    }
    backgroundSprite_.setTexture(backgroundTexture_);

    // 初始化金币显示
    coinsText_.setFont(Game::GlobalFont);
    coinsText_.setCharacterSize(40);
    coinsText_.setFillColor(sf::Color::Yellow);
    coinsText_.setStyle(sf::Text::Bold);
    
    // 加载商店数据以获取当前金币数
    StoreScreen::LoadGameData();
    UpdateCoinsDisplay();
}

void MenuScreen::UpdateCoinsDisplay()
{
    coinsText_.setString(L"金币: " + std::to_wstring(StoreScreen::playerCoins));
    sf::FloatRect textBounds = coinsText_.getLocalBounds();
    coinsText_.setOrigin(textBounds.left + textBounds.width / 2.0f,
                        textBounds.top + textBounds.height / 2.0f);
    coinsText_.setPosition(Game::GlobalVideoMode.width / 2.f,
                          Game::GlobalVideoMode.height * 0.1f);
}

void MenuScreen::handleInput(sf::RenderWindow &window)
{
    static sf::Vector2i mousePosition;
    mousePosition = sf::Mouse::getPosition(window);

    for (auto &i : button_)
        i.focused(false);
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
            Game::mouseButtonLocked = true;
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::MainScreen = std::make_shared<GameScreen>();
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

    if (button_[3].contain(mousePosition))
    {
        button_[3].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<StoreScreen>();
            return;
        }
    }
    if (button_[4].contain(mousePosition))
    {
        button_[4].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<RankScreen>();
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

void MenuScreen::update(sf::Time delta)
{
    Game::GlobalTitle.update(delta);
    UpdateCoinsDisplay(); // 实时更新金币显示
}

void MenuScreen::render(sf::RenderWindow &window)
{
    window.draw(backgroundSprite_); // 绘制背景
    window.setView(window.getDefaultView());
    Game::GlobalTitle.render(window);
    window.draw(coinsText_);  // 绘制金币显示
    for (auto &button : button_)
        button.render(window);
    helpButton_.render(window);
    aboutButton_.render(window);
}