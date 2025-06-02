#include <random>
#include <iostream>

#include "AISnake.h"
#include "Snake.h"
#include "Game.h"

#include <element/AISnake.h>

using namespace sfSnake;

const int AISnake::InitialSize = 5;

// Part0
AISnake::AISnake()
    : isDead_(false),
      score_(InitialSize),
      lastDecisionTime_(sf::Time::Zero),
      decisionInterval_(sf::milliseconds(100)), // 每100ms做一次决策
      hasTarget_(false),
      direction_(Direction(1, 0)), 
      nodeRadius_(Game::GlobalVideoMode.width / 100.0f),
      tailOverlap_(0u),
      nodeShape(nodeRadius_),
      nodeMiddle(sf::Vector2f(nodeRadius_ * std::sqrt(3), nodeRadius_)),
      playingDeathEffect_(false),
      deathEffectTime_(sf::Time::Zero),
      deathEffectDuration_(sf::seconds(3.0f))
{
    initNodes();
    // 设置AI蛇的形状和颜色
    nodeShape.setFillColor(sf::Color(0xe74c3cff)); 
    nodeMiddle.setFillColor(sf::Color(0x2c3e50ff)); 
    
    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);
    
    headTexture.loadFromFile("assets/image/snake_head1.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);
    headSprite.setColor(sf::Color(0xe74c3cff)); 
    
    setOriginMiddle(headSprite);
}

void AISnake::update(sf::Time delta, const std::deque<Fruit>& fruits, const Snake& playerSnake)
{
    // 检查与玩家的碰撞
    if (checkPlayerCollision(playerSnake)||checkBoundaryCollision()|| checkSelfCollisions()) {
        isDead_ = true;
        playingDeathEffect_ = true;
        deathEffectTime_ = sf::Time::Zero;
    
        // 创建爆炸粒子效果
        explosionParticles_.clear();
        std::default_random_engine engine(time(NULL));
        std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
        std::uniform_real_distribution<float> speedDist(50.0f, 150.0f);
        
        for (int i = 0; i < 100; i++) {
            sf::CircleShape particle(nodeRadius_ );
            particle.setFillColor(sf::Color(255, 100, 100, 255));
            particle.setPosition(getHeadPosition());
            setOriginMiddle(particle);
            explosionParticles_.push_back(particle);
        }
        return;
    }
    
    // AI决策逻辑
    lastDecisionTime_ += delta;
    if (lastDecisionTime_ >= decisionInterval_) {
        makeDecision(fruits, playerSnake);
        lastDecisionTime_ = sf::Time::Zero;
    }
    move();
}

template <typename T>
void AISnake::renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window)
{
    shape.setPosition(nowPosition);
    window.draw(shape);
}

void AISnake::render(sf::RenderWindow &window)
{
    if (isDead_) {
        if (playingDeathEffect_) {
            // 渲染爆炸粒子效果
            for (const auto& particle : explosionParticles_) {
                window.draw(particle);
            }
        }
        return;
    }
    
    static int count;
    static SnakePathNode lastSnakeNode, lastMiddleNode, nowSnakeNode;
    static float angle;
    static sf::Vector2f recDirection;

    lastSnakeNode = *path_.begin();
    headSprite.setPosition(lastSnakeNode);
    recDirection = direction_;
    angle = std::acos(recDirection.y / std::sqrt(recDirection.x * recDirection.x + recDirection.y * recDirection.y)) /
        3.14159265358979323846 * 180.0;
    if (direction_.x > 0)
        angle = -angle;
    headSprite.setRotation(angle);

    renderNode(lastSnakeNode, headSprite, window);

    count = 1;
    for (auto i = path_.begin() + 1; i != path_.end(); i++, count++)
    {
        if (count % 5 == 0)
        {
            if (count % 2)
                lastMiddleNode = *i;
            else
            {
                nowSnakeNode = *i;

                recDirection = nowSnakeNode - lastSnakeNode;
                angle = std::acos(recDirection.y / std::sqrt(recDirection.x * recDirection.x + recDirection.y * recDirection.y)) /
                    3.14159265358979323846 * 180.0;
                if (recDirection.x > 0)
                    angle = -angle;
                nodeMiddle.setRotation(angle);

                renderNode(nowSnakeNode, nodeShape, window);
                renderNode(lastMiddleNode, nodeMiddle, window);

                lastSnakeNode = nowSnakeNode;
            }
        }
    }
}

// Part1.检查相关
void AISnake::checkFruitCollisions(std::deque<Fruit> &fruits)
{
    if (isDead_) return;
    
    auto toRemove = fruits.end();
    SnakePathNode headnode = path_.front();

    for (auto i = fruits.begin(); i != fruits.end(); ++i)
    {
        if (distance(i->shape_.getPosition(), headnode) < 
            nodeRadius_ + i->shape_.getRadius())
            toRemove = i;
    }

    if (toRemove != fruits.end())
    {
        grow(toRemove->score_);
        fruits.erase(toRemove);
    }
}

bool AISnake::checkPlayerCollision(const Snake& playerSnake)
{
    sf::Vector2f aiHead = getHeadPosition();
    
    // 检查AI蛇头与玩家蛇身的碰撞
    for (const auto& playerNode : playerSnake.getBodyNodes()) {
        if (distance(aiHead, playerNode) < 1.8f * nodeRadius_) {
            return true;
        }
    }
    
    return false;
}

bool AISnake::checkSelfCollisions()
{
    if (isDead_) return true;
    
    SnakePathNode head = path_.front();
    int count = 0;

    for (auto i = path_.begin(); i != path_.end(); i++, count++)
        if (count >= 30 && distance(head, *i) < 2.0f * nodeRadius_)
        {
            isDead_ = true;
            return true; 
        }
    return false;
}

// 检查蛇头是否在地图边界内
bool AISnake::isWithinMapBounds(const sf::Vector2f& pos, float nodeRadius) const {
    return pos.x >= nodeRadius && pos.x <= Game::mapWidth - nodeRadius &&
           pos.y >= nodeRadius && pos.y <= Game::mapHeight - nodeRadius;
}

bool AISnake::checkBoundaryCollision() const
{
    sf::Vector2f headPos = getHeadPosition();

    if (!isWithinMapBounds(headPos, nodeRadius_)) {
        return true; // 碰撞到边界

    }
    return false; // 没有碰撞到边界
}

// Part2.行为相关
void AISnake::move()
{
    if (isDead_) return;
    
    SnakePathNode &headNode = path_.front();
    path_.push_front(SnakePathNode(
        headNode.x + direction_.x * nodeRadius_ / 5.0,
        headNode.y + direction_.y * nodeRadius_ / 5.0));
    
    if (tailOverlap_)
        tailOverlap_--;
    else
        path_.pop_back();
}

void AISnake::grow(int score)
{
    tailOverlap_ += score * 10;
    score_ += score;
}

unsigned AISnake::getScore() const
{
    return score_;
}

// Part3.决策相关
void AISnake::makeDecision(const std::deque<Fruit>& fruits, const Snake& playerSnake)
{
    // 1. 获取所有可能的方向
    std::vector<Direction> allDirections = {
        Direction(0, -1), // 上
        Direction(0, 1),  // 下
        Direction(-1, 0), // 左
        Direction(1, 0)   // 右
    };
    
    // 2. 评估每个方向的安全性和价值
    std::vector<std::pair<Direction, float>> directionScores;
    
    for (const auto& dir : allDirections) {
        // 避免180度转向（蛇不能直接反向）
        if (direction_.x != 0 && dir.x == -direction_.x && dir.y == direction_.y) continue;
        if (direction_.y != 0 && dir.y == -direction_.y && dir.x == direction_.x) continue;
        
        float score = evaluateDirection(dir, fruits, playerSnake);
        if (score > -1000) { // 只考虑不会立即死亡的方向
            directionScores.push_back({dir, score});
        }
    }
    
    // 3. 如果没有安全方向，使用紧急避险策略
    if (directionScores.empty()) {
        Direction emergencyDir = findEmergencyDirection(playerSnake);
        direction_ = emergencyDir;
        hasTarget_ = false;
        return;
    }
    
    // 4. 按分数排序，选择最好的方向
    std::sort(directionScores.begin(), directionScores.end(),
        [](const std::pair<Direction, float>& a, const std::pair<Direction, float>& b) {
            return a.second > b.second;
        });
    
    direction_ = directionScores[0].first;
    
    // 5. 更新目标果子
    sf::Vector2f nearestFruit = findNearestFruit(fruits);
    if (nearestFruit.x != -1 && nearestFruit.y != -1) {
        targetFruit_ = nearestFruit;
        hasTarget_ = true;
    } else {
        hasTarget_ = false;
    }
}

float AISnake::evaluateDirection(const Direction& dir, const std::deque<Fruit>& fruits, const Snake& playerSnake)
{
    float score = 0.0f;
    sf::Vector2f headPos = getHeadPosition();

    // 1. 基础安全检查 - 如果不安全，返回极低分数
    if (!isDirectionSafeAdvanced(dir, playerSnake)) {
        return -1000.0f;
    }
    
    // 2. 距离边界的安全距离奖励
    sf::Vector2f futurePos = headPos + dir * (nodeRadius_ * 2.0f);
    float distToWall = std::min({
        futurePos.x,
        Game::mapWidth - futurePos.x,
        futurePos.y,
        Game::mapHeight - futurePos.y
    });
    score += std::min(distToWall / nodeRadius_, 10.0f) * 5.0f; // 离墙越远分数越高
    
    // 3. 朝向果子的方向奖励
    if (!fruits.empty()) {
        sf::Vector2f nearestFruit = findNearestFruit(fruits);
        if (nearestFruit.x != -1) {
            sf::Vector2f toFruit = nearestFruit - headPos;
            float dot = (dir.x * toFruit.x + dir.y * toFruit.y) / 
                       std::sqrt(toFruit.x * toFruit.x + toFruit.y * toFruit.y);
            score += dot * 20.0f; // 朝向果子的方向得分更高
        }
    }
    
    // 4. 避免困在角落 - 计算可达空间
    float freeSpace = calculateFreeSpace(futurePos, dir, playerSnake);
    score += freeSpace * 2.0f;

    if (freeSpace < static_cast<float>(path_.size())) {
        score -= (path_.size() - freeSpace) * 10.0f;
    }
    
    // 5. 避免与玩家过于接近
    float distToPlayer = getDistanceToPlayer(futurePos, playerSnake);
    if (distToPlayer < nodeRadius_ * 8.0f) {
        score -= (nodeRadius_ * 8.0f - distToPlayer) * 0.5f;
    }
    
    //  避免撞到自己身体
for (const auto& segment : path_) {
    float dist = distance(futurePos, segment);
    if (dist < nodeRadius_ * 2.0f) {
        score -= (nodeRadius_ * 2.0f - dist) * 500.0f; // 离自己越近惩罚越大
    }
}

    return score;
}

bool AISnake::isDirectionSafeAdvanced(const Direction& dir, const Snake& playerSnake)
{
    // 检查更多步数以确保安全
    int checkSteps = 30;
    
    // 检查是否会撞墙
    if (willCollideWithBoundary(dir, checkSteps)) return false;
    
    // 检查是否会撞到自己
    if (willCollideWithSelf(dir, checkSteps)) return false;
    
    // 检查是否会撞到玩家
    if (willCollideWithPlayer(dir, playerSnake, checkSteps)) return false;
    
    return true;
}

float AISnake::calculateFreeSpace(const sf::Vector2f& pos, const Direction& dir, const Snake& playerSnake)
{
    // 洪水填充算法计算可达空间
    float freeSpace = 0.0f;
    int maxDistance = 20;
    
    for (int i = 1; i <= maxDistance; i++) {
        sf::Vector2f testPos = pos + dir * (float)(i * nodeRadius_ / 2.0);
        
        // 检查这个位置是否可达
        if (testPos.x < nodeRadius_ || testPos.x > Game::mapWidth - nodeRadius_ ||
            testPos.y < nodeRadius_ || testPos.y > Game::mapHeight - nodeRadius_) {
            break;
        }
        
        // 检查是否与自己身体碰撞
        bool collision = false;
        for (const auto& node : path_) {
            if (distance(testPos, node) < 1.5f * nodeRadius_) {
                collision = true;
                break;
            }
        }
        
        if (collision) break;
        
        // 检查是否与玩家碰撞
        for (const auto& playerNode : playerSnake.getBodyNodes()) {
            if (distance(testPos, playerNode) < 1.5f * nodeRadius_) {
                collision = true;
                break;
            }
        }
        
        if (collision) break;
        
        freeSpace += 1.0f;
    }
    
    return freeSpace;
}

float AISnake::getDistanceToPlayer(const sf::Vector2f& pos, const Snake& playerSnake)
{
    float minDist = std::numeric_limits<float>::max();
    
    for (const auto& playerNode : playerSnake.getBodyNodes()) {
        float dist = distance(pos, playerNode);
        minDist = std::min(minDist, dist);
    }
    
    return minDist;
}

Direction AISnake::findEmergencyDirection(const Snake& playerSnake)
{
    // 紧急情况下，选择能存活最久的方向
    std::vector<Direction> allDirections = {
        Direction(0, -1), // 上
        Direction(0, 1),  // 下
        Direction(-1, 0), // 左
        Direction(1, 0)   // 右
    };
    
    Direction bestDir = direction_; // 默认保持当前方向
    int maxSurvivalSteps = 0;
    
    for (const auto& dir : allDirections) {
        // 避免180度转向
        if (direction_.x != 0 && dir.x == -direction_.x && dir.y == direction_.y) continue;
        if (direction_.y != 0 && dir.y == -direction_.y && dir.x == direction_.x) continue;
        
        int survivalSteps = calculateSurvivalSteps(dir, playerSnake);
        if (survivalSteps > maxSurvivalSteps) {
            maxSurvivalSteps = survivalSteps;
            bestDir = dir;
        }
    }
    
    return bestDir;
}

int AISnake::calculateSurvivalSteps(const Direction& dir, const Snake& playerSnake)
{
    sf::Vector2f currentPos = getHeadPosition();
    int steps = 0;
    
    // 模拟向这个方向移动，看能存活多少步
    for (int i = 1; i <= 50; i++) {
        sf::Vector2f testPos = currentPos + dir * (float)(i * nodeRadius_ / 5.0);
        
        // 检查边界
        if (testPos.x < nodeRadius_ || testPos.x > Game::mapWidth - nodeRadius_ ||
            testPos.y < nodeRadius_ || testPos.y > Game::mapHeight - nodeRadius_) {
            break;
        }
        
        // 检查自身碰撞
        bool collision = false;
        int count = 0;
        for (const auto& node : path_) {
            if (count >= 30 && distance(testPos, node) < 1.8f * nodeRadius_) {
                collision = true;
                break;
            }
            count++;
        }
        
        if (collision) break;
        
        // 检查玩家碰撞
        for (const auto& playerNode : playerSnake.getBodyNodes()) {
            if (distance(testPos, playerNode) < 1.8f * nodeRadius_) {
                collision = true;
                break;
            }
        }
        
        if (collision) break;
        
        steps = i;
    }
    
    return steps;
}

sf::Vector2f AISnake::findNearestFruit(const std::deque<Fruit>& fruits)
{
    sf::Vector2f headPos = getHeadPosition();
    sf::Vector2f nearest(-1, -1);
    float minDistance = std::numeric_limits<float>::max();
    
    for (const auto& fruit : fruits) {
        sf::Vector2f fruitPos = fruit.shape_.getPosition();
        float dist = distance(headPos, fruitPos);
        
        // 优先选择有分数的果子，距离权重调整
        float priority = dist;
        if (fruit.score_ > 0) {
            priority *= 0.7f; // 有分数的果子优先级更高
        }
        
        if (priority < minDistance) {
            minDistance = priority;
            nearest = fruitPos;
        }
    }
    
    return nearest;
}

bool AISnake::isDirectionSafe(const Direction& dir, const Snake& playerSnake)
{
    // 检查是否会撞墙
    if (willCollideWithBoundary(dir, 5)) return false;
    
    // 检查是否会撞到自己
    if (willCollideWithSelf(dir, 5)) return false;
    
    // 检查是否会撞到玩家
    if (willCollideWithPlayer(dir, playerSnake, 5)) return false;
    
    return true;
}

bool AISnake::willCollideWithSelf(const Direction& dir, int steps)
{
    sf::Vector2f headPos = getHeadPosition();
    
    for (int step = 1; step <= steps; step++) {
        sf::Vector2f futurePos = headPos + dir * (float)(step * nodeRadius_ / 5.0);
        
        int count = 0;
        for (const auto& node : path_) {
            if (count >= 30 && distance(futurePos, node) < 2.0f * nodeRadius_) {
                return true;
            }
            count++;
        }
    }
    return false;
}

bool AISnake::willCollideWithBoundary(const Direction& dir, int steps)
{
    sf::Vector2f headPos = getHeadPosition();
    sf::Vector2f futurePos = headPos + dir * (float)(steps * nodeRadius_ / 5.0);
    
    return (futurePos.x < 5 || futurePos.x > Game::mapWidth - 5 ||
            futurePos.y < 5 || futurePos.y > Game::mapHeight - 5);
}

bool AISnake::willCollideWithPlayer(const Direction& dir, const Snake& playerSnake, int steps)
{
    sf::Vector2f headPos = getHeadPosition();
    
    for (int step = 1; step <= steps; step++) {
        sf::Vector2f futurePos = headPos + dir * (float)(step * nodeRadius_ / 5.0);
        
        // 检查与玩家蛇身的碰撞
        for (const auto& playerNode : playerSnake.getBodyNodes()) {
            if (distance(futurePos, playerNode) < 2.0f * nodeRadius_) {
                return true;
            }
        }
    }
    return false;
}

float AISnake::distance(const sf::Vector2f& a, const sf::Vector2f& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

Direction AISnake::normalizeDirection(const sf::Vector2f& dir)
{
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length == 0) return Direction(0, 0);
    return Direction(dir.x / length, dir.y / length);
}

// Part4.蛇相关
const sf::Vector2f AISnake::getHeadPosition() const {
            if(!path_.empty()){
                return path_.front();
            } else {
                return sf::Vector2f(0.f, 0.f);
            }
        }
const std::deque<SnakePathNode>& AISnake::getBodyNodes() const {
            return path_;
        }
void AISnake::initNodes()
{
    path_.clear();
    // AI蛇从左上角开始
    sf::Vector2f startPos(Game::GlobalVideoMode.width / 3.0f, Game::GlobalVideoMode.height / 3.0f);
    
    path_.push_back(startPos);
    for (int i = 1; i <= 10 * InitialSize; i++)
    {
        path_.push_back(SnakePathNode(
            startPos.x - direction_.x * i * nodeRadius_ / 5.0,
            startPos.y - direction_.y * i * nodeRadius_ / 5.0));
    }
}









void AISnake::updateDeathEffect(sf::Time delta)
{
    deathEffectTime_ +=  delta;
    
    if (deathEffectTime_ >= deathEffectDuration_) {
        playingDeathEffect_ = false;
        return;
    }
    
    // 更新粒子效果
    float progress = deathEffectTime_.asSeconds() / deathEffectDuration_.asSeconds();
    
    for (size_t i = 0; i < explosionParticles_.size(); i++) {
        float angle = (2 * 3.14159f * i) / explosionParticles_.size();
        float speed = 100.0f * (1.0f - progress);
        
        sf::Vector2f offset(
            std::cos(angle) * speed * progress,
            std::sin(angle) * speed * progress
        );
        
        explosionParticles_[i].setPosition(getHeadPosition() + offset);
        
        // 淡出效果
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.0f - progress));
        explosionParticles_[i].setFillColor(sf::Color(255, 100, 100, alpha));
    }
}












