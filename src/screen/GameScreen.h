#pragma once

#include <SFML/Graphics.hpp>
#include <deque>

#include "screen/Screen.h"

#include "element/Snake.h"
#include "element/AISnake.h"
#include "element/Fruit.h"
#include "element/Grid.h"
#include "element/Button.h"

namespace sfSnake
{
    class GameScreen : public Screen
    {
    public:
        GameScreen();
        //游戏相关
        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;
        bool gameOver_ = false;

        //蛇相关
        Snake snake_;
        AISnake AISnake_;
        //获取蛇对象
        AISnake& getAISnake() { return AISnake_; }
        Snake& getPlayerSnake() { return snake_; }

        //水果相关
        void generateFruit();
        static std::deque<Fruit> fruit_;



    // 地图相关
    // 地图边界
        static void setupborderline(sf::RectangleShape& border){
            border.setSize(sf::Vector2f(2000,1600));
            border.setPosition(0.f, 0.f); 
            border.setFillColor(sf::Color::Transparent); // 透明填充
            border.setOutlineThickness(-5.f);             // 边框粗细
            border.setOutlineColor(sf::Color::Red);  
        }
    // 地图网格
        Grid grid_;
    // 背景
        Button pauseButton_;
    // 摇杆相关
        sf::Text score_;
        sf::CircleShape baseCircle;   // 摇杆底座
        sf::CircleShape stick;        // 摇杆杆头
        sf::Vector2f basePos;         // 摇杆中心位置
        sf::Vector2f direction;       // 当前控制方向（归一化向量）

    };
};

