#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <deque>

#include "Fruit.h"
#include "AISnake.h"
namespace sfSnake
{
    typedef sf::Vector2f Direction; 
    typedef sf::Vector2f SnakePathNode;
    class Snake
    {
    public:
    // Part0
        Snake();

        void handleInput(sf::RenderWindow &window);
        void update(const AISnake&AISnake_);
        template <typename T>
        void renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window);
        void render(sf::RenderWindow &window);


    // Part1.分数相关
        unsigned getScore() const;
        unsigned int score_;

        void printhead() const;

    // Part2.获取蛇位置
        const sf::Vector2f getHeadPosition() const;
        const std::deque<SnakePathNode>& getBodyNodes() const;
    // Part3.行为相关
        void move();
        void grow(int score);
        bool speedup_;


    // Part4.检查相关
        void checkFruitCollisions(std::deque<Fruit> &fruits);
        void checkOutOfWindow();
        void checkSelfCollisions();
        bool checkAICollision(const AISnake& AISnake); // 检查是否被AI撞死
        bool hitSelf() const;
        bool hitSelf_;




    // Part5.蛇相关
        void initNodes();
        Direction direction_;
        Direction newdirection_;
        float nodeRadius_;
        std::deque<SnakePathNode> path_; 
        unsigned tailOverlap_;

        sf::CircleShape nodeShape;
        sf::RectangleShape nodeMiddle;
        sf::Texture headTexture;
        sf::Sprite headSprite;
        
    // Part6.换皮肤
        void changeSkin(int selectedSkin, sf::Sprite &headSprite, sf::Texture &headTexture,
                       float nodeRadius_);

        static const int InitialSize;
    };
}