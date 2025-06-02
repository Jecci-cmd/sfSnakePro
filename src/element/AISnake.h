#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>

#include "Fruit.h"

namespace sfSnake
{
    typedef sf::Vector2f Direction; 
    typedef sf::Vector2f SnakePathNode;
    
    class Snake; // 前向声明，用于碰撞检测
    
    class AISnake
    {
    public:
    // Part0
        AISnake();

        void update(sf::Time delta, const std::deque<Fruit>& fruits, const Snake& playerSnake);
        template <typename T>
        void renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window);
        void render(sf::RenderWindow &window);

    // Part1.检查相关
        void checkFruitCollisions(std::deque<Fruit> &fruits);
         // 检查是否被玩家撞死
        bool checkPlayerCollision(const Snake& playerSnake);
         // 检查是否自撞
        bool checkSelfCollisions();
        // 检查是否撞到边界
        bool isWithinMapBounds(const sf::Vector2f& pos, float nodeRadius) const;
        bool checkBoundaryCollision() const;
        bool isDead() const { return isDead_; }
        bool isDead_;

    // Part2.行为相关
        void move();
        void grow(int score);
        unsigned getScore() const;
        unsigned int score_;
       
    // Part3.决策相关
        void makeDecision(const std::deque<Fruit>& fruits, const Snake& playerSnake);
        float evaluateDirection(const Direction& dir, const std::deque<Fruit>& fruits, const Snake& playerSnake);
        bool isDirectionSafeAdvanced(const Direction& dir, const Snake& playerSnake);
        float calculateFreeSpace(const sf::Vector2f& pos, const Direction& dir, const Snake& playerSnake);
        float getDistanceToPlayer(const sf::Vector2f& pos, const Snake& playerSnake);
        Direction findEmergencyDirection(const Snake& playerSnake);
        int calculateSurvivalSteps(const Direction& dir, const Snake& playerSnake);
        sf::Vector2f findNearestFruit(const std::deque<Fruit>& fruits);
        bool isDirectionSafe(const Direction& dir, const Snake& playerSnake);
        bool willCollideWithSelf(const Direction& dir, int steps = 3);
        bool willCollideWithBoundary(const Direction& dir, int steps = 3);
        bool willCollideWithPlayer(const Direction& dir, const Snake& playerSnake, int steps = 3);

        // 辅助方法
        float distance(const sf::Vector2f& a, const sf::Vector2f& b);
        Direction normalizeDirection(const sf::Vector2f& dir);

        // AI决策参数
        sf::Time lastDecisionTime_;
        sf::Time decisionInterval_; // AI决策间隔时间
        sf::Vector2f targetFruit_; // 当前目标果子
        bool hasTarget_;
    
    // Part4.蛇相关
        const sf::Vector2f getHeadPosition() const;
        const std::deque<SnakePathNode>& getBodyNodes() const;
        void initNodes();
        static const int InitialSize;
        Direction direction_;
        float nodeRadius_;
        std::deque<SnakePathNode> path_;
        unsigned tailOverlap_;
        sf::CircleShape nodeShape;
        sf::RectangleShape nodeMiddle;
        sf::Texture headTexture;
        sf::Sprite headSprite;
        
       
        
        
    // Part5.死亡特效相关
        bool isPlayingDeathEffect() const { return playingDeathEffect_; }
        void updateDeathEffect(sf::Time delta);
        bool playingDeathEffect_;
        sf::Time deathEffectTime_;
        sf::Time deathEffectDuration_;
        std::vector<sf::CircleShape> explosionParticles_;
    };
}

