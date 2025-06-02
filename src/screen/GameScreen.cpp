#include <SFML/Graphics.hpp>

#include <random>
#include <memory>
#include <iostream>

#include "screen/GameScreen.h"
#include "screen/GameOverScreen.h"
#include "screen/PauseScreen.h"
#include "screen/HelpScreen.h"

using namespace sfSnake;
std::deque<sfSnake::Fruit> sfSnake::GameScreen::fruit_;

GameScreen::GameScreen()
    : snake_(), AISnake_(), grid_(), pauseButton_()
{
    pauseButton_.update("assets/image/pauseUI.png", 170.f);
    pauseButton_.setPosition(
        Game::GlobalVideoMode.width / 15.0 * 14.0,
        Game::GlobalVideoMode.width / 15.0);

    score_.setFont(Game::GlobalFont);
    score_.setString(sf::String(L"金币数:") + std::to_string(snake_.getScore()));
    score_.setCharacterSize(Game::GlobalVideoMode.width / 25.0f);
    score_.setFillColor(Game::Color::Yellow);
    setOriginMiddle(score_);
    score_.setPosition( //
        Game::GlobalVideoMode.width / 2.0f,
        Game::GlobalVideoMode.width * 0.05f);
        basePos = sf::Vector2f(100.f, Game::GlobalVideoMode.height - 100.f); // 左下角位置

        baseCircle.setRadius(80.f);
        baseCircle.setFillColor(sf::Color(50,50,50)); 
        baseCircle.setOrigin(80.f, 80.f);
        baseCircle.setPosition(basePos);
        
        stick.setRadius(40.f); //摇杆头
        stick.setFillColor(sf::Color(255,255,255));
        stick.setOrigin(40.f, 40.f); // 设置原点为中心
        stick.setPosition(basePos);//初始位置在圆心
        
        direction = sf::Vector2f(0.f, 0.f);     //初始化方向向量为（0，0）
}

void GameScreen::handleInput(sf::RenderWindow &window)
{
    snake_.handleInput(window);

    auto mousePosition = sf::Mouse::getPosition(window);

    pauseButton_.focused(false);
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)); // 获取鼠标在窗口内的位置
    sf::Vector2f delta = mousePos - basePos;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if (distance < 100.f) {
        stick.setPosition(mousePos);
    } else {
        sf::Vector2f limited = basePos + 100.f * delta / distance;
        stick.setPosition(limited);
    }
    direction = delta / distance; // 归一化方向
} else {
    stick.setPosition(basePos); // 松开复位
    direction = sf::Vector2f(0.f, 0.f);
}


    if (pauseButton_.contain(mousePosition))
    {
        pauseButton_.focused(true);
        if (
            !Game::mouseButtonLocked &&
            sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpGameScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<PauseScreen>();
            return;
        }
    }

    
}

void GameScreen::update(sf::Time delta)
{
    while (fruit_.size() < 60)
        generateFruit();

    snake_.update(AISnake_);
    snake_.checkFruitCollisions(fruit_);
    
    // AI蛇的更新和碰撞检测
    if (!AISnake_.isDead()){// 如果AI蛇已经死亡，则不再更新
    AISnake_.update(delta, fruit_, snake_);
    AISnake_.checkFruitCollisions(fruit_);}
    else if (AISnake_.isPlayingDeathEffect())
    AISnake_.updateDeathEffect(delta); 


    if (snake_.hitSelf()) // 检查玩家蛇和AI蛇是否碰撞
    {
        Game::TmpScreen = Game::MainScreen;
        int score = snake_.getScore();
        Game::MainScreen = std::make_shared<GameOverScreen>(score);
        return;
    }


    score_.setString(sf::String(L"金币数:\t") + std::to_string(snake_.getScore()));
}

void GameScreen::render(sf::RenderWindow &window) // 区分地图视图和UI视图
{
    // 游戏视图绘制
    if (Game::GridVisibility) // 背景网格层
        grid_.render(window);
    snake_.render(window); 
    AISnake_.render(window); 
    for (auto& fruit : fruit_) 
        fruit.render(window);
    sf::RectangleShape border;
    setupborderline(border);
    window.draw(border);
     
    
    // 小地图视图绘制
    // 设置地图缩略视图大小
    sf::View miniMapView;
    miniMapView.setSize(2000.f, 1600.f);  // 你的大地图尺寸
    miniMapView.setCenter(1000.f, 800.f); // 固定中心
    miniMapView.setViewport(sf::FloatRect(0.f, 0.f, 0.15f, 0.25f)); // 屏幕左上角

    window.setView(miniMapView);

    // 获取两个蛇头的位置
    sf::Vector2f playerHeadPos = snake_.getHeadPosition();   
    sf::Vector2f aiHeadPos     = AISnake_.getHeadPosition(); 

    // (3) 画两个蛇头
    sf::CircleShape playerDot(100.f); 
    playerDot.setFillColor(sf::Color::Green);
    playerDot.setOutlineColor(sf::Color::Green); 
    playerDot.setOutlineThickness(3.f);
    playerDot.setOrigin(40.f, 40.f);
    playerDot.setPosition(playerHeadPos);
    window.draw(playerDot);

    sf::CircleShape aiDot(100.f);
    aiDot.setFillColor(sf::Color::Red);
    aiDot.setOutlineColor(sf::Color::Red); 
    aiDot.setOutlineThickness(3.f);
    aiDot.setOrigin(40.f, 40.f);
    aiDot.setPosition(aiHeadPos);
    window.draw(aiDot);

    
    window.setView(window.getDefaultView()); // 切回默认视图，绘制UI
    //地图边框
    sf::RectangleShape miniMapBorder(sf::Vector2f(Game::GlobalVideoMode.width * 0.15f, Game::GlobalVideoMode.height * 0.25f));
    miniMapBorder.setPosition(0.f, 0.f);
    miniMapBorder.setFillColor(sf::Color::Transparent);
    miniMapBorder.setOutlineColor(sf::Color::Yellow);
    miniMapBorder.setOutlineThickness(2.f);
    window.draw(miniMapBorder);

    // UI视图绘制
    pauseButton_.render(window); // 绘制暂停键
    window.draw(score_); // 绘制得分
    window.draw(baseCircle); // 绘制摇杆
    window.draw(stick);


}

void GameScreen::generateFruit()
{
    static std::default_random_engine engine(time(NULL));
    static std::default_random_engine colorEngine(time(NULL));

    static std::uniform_int_distribution<int> xPos( // 水果坐标
        Game::GlobalVideoMode.width*2 / 15.0f,
        Game::GlobalVideoMode.width*2 -
            Game::GlobalVideoMode.width*2 / 10.0f);

    static std::uniform_int_distribution<int> yPos(
        Game::GlobalVideoMode.width*2 / 10.0f,
        Game::GlobalVideoMode.height*2 -
            Game::GlobalVideoMode.width*2 / 15.0f);

    static std::uniform_int_distribution<int> fruitColor(0, 7);

    switch (fruitColor(colorEngine))
    {
    case 0: // black
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[0],
            0));
        break;
    case 1: // brown
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[1],
            0));
        break;
    case 2:
    case 3: // red
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[2],
            3));
        break;
    case 4:
    case 5: // blue
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[3],
            2));
        break;
    case 6:
    case 7: // green
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[4],
            1));
        break;
    default:
        break;
    }
}



