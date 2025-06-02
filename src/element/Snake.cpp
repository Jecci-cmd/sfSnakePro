#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Game.h"

#include "element/Snake.h"
#include "element/Fruit.h"
#include "element/AISnake.h"

#include "screen/GameOverScreen.h"
#include "screen/StoreScreen.h"


using namespace sfSnake;

const int Snake::InitialSize = 5;
// Part0
Snake::Snake()
    : score_(InitialSize),
      speedup_(false),
      hitSelf_(false),
      direction_(Direction(0, -1)),
      newdirection_(Direction(0,-1)),
      nodeRadius_(Game::GlobalVideoMode.width / 100.0f),
      tailOverlap_(0u),
      nodeShape(nodeRadius_),
      nodeMiddle(sf::Vector2f(nodeRadius_ * std::sqrt(3), nodeRadius_))
{
    initNodes();
    changeSkin(StoreScreen::selectedSkin, headSprite, headTexture, nodeRadius_);
}

void Snake::handleInput(sf::RenderWindow &window)
{
    if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        newdirection_ = Direction(0, -1);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        newdirection_ = Direction(0, 1);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        newdirection_ = Direction(-1, 0);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        newdirection_ = Direction(1, 0);
     if (!(newdirection_.x == -direction_.x && newdirection_.y == -direction_.y)) {
            direction_ = newdirection_;
        }

    static double directionSize;

    if (!Game::mouseButtonLocked)
    {
        if (
            sf::Mouse::isButtonPressed(sf::Mouse::Left) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            static sf::Vector2i MousePosition;
            MousePosition = sf::Mouse::getPosition(window);
            if (
                dis(MousePosition,
                    sf::Vector2f(
                        Game::GlobalVideoMode.width / 15.0f * 14.0f,
                        Game::GlobalVideoMode.width / 15.0f)) >
                Game::GlobalVideoMode.width / 16.0f)
            {
                direction_ =
                    static_cast<sf::Vector2f>(MousePosition) -
                   path_.front();
                directionSize = length(direction_);
                direction_.x /= directionSize;
                direction_.y /= directionSize;
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        speedup_ = true;
    else
        speedup_ = false;
}

void Snake::update(const AISnake&AISnake_)
{
    move();
    checkOutOfWindow();
    checkSelfCollisions();
    checkAICollision(AISnake_);
}

template <typename T>
void Snake::renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window)
{
    shape.setPosition(nowPosition);
    window.draw(shape);

}

void Snake::render(sf::RenderWindow &window)
{
    static int count;
    static SnakePathNode lastSnakeNode, lastMiddleNode, nowSnakeNode;
    static float angle;
    static sf::Vector2f recDirection;
    static SnakePathNode wNowHeadNode;

    lastSnakeNode = *path_.begin();// 获取蛇头节点
     headSprite.setPosition(lastSnakeNode); 
    recDirection = direction_;
    angle =// 计算蛇头方向与y轴的夹角
        std::acos(recDirection.y / length(recDirection)) /
        3.14159265358979323846 * 180.0;
    if (direction_.x > 0)
        angle = -angle;
    count = 1;
    headSprite.setRotation(angle);
    renderNode(lastSnakeNode, headSprite, window);
    for (auto i = path_.begin() + 1; i != path_.end(); i++, count++)
    {
        if (count % 5 == 0) //每5个节点渲染一个，节省渲染资源
        {
            if (count % 2)
                lastMiddleNode = *i;
            else
            {
                nowSnakeNode = *i;

                recDirection = nowSnakeNode - lastSnakeNode;
                angle =
                    std::acos(recDirection.y / length(recDirection)) /
                    3.14159265358979323846 * 180.0;
                if (recDirection.x > 0)
                    angle = -angle;
                nodeMiddle.setRotation(angle);

                static SnakePathNode wNowSnakeNode;
                static SnakePathNode wMiddleNode;
                renderNode(nowSnakeNode, nodeShape, window);
                renderNode(lastMiddleNode, nodeMiddle, window);

                lastSnakeNode = nowSnakeNode;
            }
        }
    }
    
}

// Part1.分数相关
unsigned Snake::getScore() const
{
    return score_;
}

// Part2.获取蛇位置
    const sf::Vector2f Snake::getHeadPosition() const{
            if(!path_.empty()){
                return path_.front();
            }else{
                return sf::Vector2f(0.f,0.f);
            }
        }

    const std::deque<SnakePathNode>& Snake::getBodyNodes() const {
        return path_;
        }

// Part3.行为相关
void Snake::move()
{
    SnakePathNode &headNode = path_.front();
    int times = speedup_ ? 2 : 1;
    for (int i = 1; i <= times; i++)
    {
        path_.push_front(SnakePathNode(
            headNode.x + direction_.x * i * nodeRadius_ / 5.0,
            headNode.y + direction_.y * i * nodeRadius_ / 5.0));
        if (tailOverlap_)
            tailOverlap_--;
        else
            path_.pop_back();
    }
}

void Snake::grow(int score)
{
    tailOverlap_ += score * 10;
    score_ += score;
}

// Part4.检查相关
void Snake::checkFruitCollisions(std::deque<Fruit> &fruits)
{
    auto toRemove = fruits.end();
    SnakePathNode headnode = path_.front();

    for (auto i = fruits.begin(); i != fruits.end(); ++i)
    {
        if (dis(
                i->shape_.getPosition(), headnode) < 
            nodeRadius_ + i->shape_.getRadius())
            toRemove = i;
    }

    if (toRemove != fruits.end())
    {
        grow(toRemove->score_);
        fruits.erase(toRemove);
    }
}

void Snake::checkOutOfWindow(){
    if(path_.front().x<5||path_.front().x>Game::mapWidth-5||
       path_.front().y<5||path_.front().y>Game::mapHeight-5){
        sf::sleep(sf::seconds(1.f));
        hitSelf_ = true;
    }
}

void Snake::checkSelfCollisions()
{
    SnakePathNode head = path_.front();
    int count = 0;

    for (auto i = path_.begin(); i != path_.end(); i++, count++)
        if (count >= 30 && dis(head, *i) < 2.0f * nodeRadius_)
        {
            sf::sleep(sf::seconds(1.f));
            hitSelf_ = true;
            break;
        }
}

bool Snake::checkAICollision(const AISnake& AISnake) {
    if(AISnake.isDead()) {
        return false;} // AI蛇已经死亡，跳过碰撞检查
      for (const auto& AINode : AISnake.getBodyNodes()) {
        if (dis(path_.front(), AINode) < 2.0f * nodeRadius_) {
            hitSelf_ = true;
            return true; 
        }
    }

    return false; 
}

bool Snake::hitSelf() const
{
    return hitSelf_;
}

// Part5.蛇相关
void Snake::initNodes()
{
    path_.push_back(SnakePathNode(
        Game::GlobalVideoMode.width ,
        Game::GlobalVideoMode.height ));
    for (int i = 1; i <= 10 * InitialSize; i++)
    {
        path_.push_back(SnakePathNode(
            Game::GlobalVideoMode.width  -
                direction_.x * i * nodeRadius_ / 5.0,
            Game::GlobalVideoMode.height  -
                direction_.y * i * nodeRadius_ / 5.0));
    }
}

// Part6.换皮肤
void Snake::changeSkin(int selectedSkin, sf::Sprite &headSprite, sf::Texture &headTexture,
                       float nodeRadius_)
{
    switch (selectedSkin) // 这里的selectedSkin是一个整数，表示选择的皮肤编号
    {
    case 0: // 默认皮肤
    {nodeShape.setFillColor(sf::Color(0xf1c40fff));
    nodeMiddle.setFillColor(sf::Color(0x1c2833ff));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snake_head1.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);

    setOriginMiddle(headSprite);
        break;}
    case 1: 
    {nodeShape.setFillColor(sf::Color(0xD3D3D3FF));
    nodeMiddle.setFillColor(sf::Color(0x2F2F2FFF));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snake_head2.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);

    setOriginMiddle(headSprite);
        break;}
    case 2:
    {nodeShape.setFillColor(sf::Color(0xFF8C00FF));
    nodeMiddle.setFillColor(sf::Color(0xFFA500FF));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snake_head3.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);
    setOriginMiddle(headSprite);
        break;}
    case 3: 
    {nodeShape.setFillColor(sf::Color(0xFFC300FF));
    nodeMiddle.setFillColor(sf::Color(0xFF8C00FF));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snake_head4.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);

    setOriginMiddle(headSprite);
        break;}
    case 4:
    {nodeShape.setFillColor(sf::Color(0xFFC0CBFF));
    nodeMiddle.setFillColor(sf::Color(0xff69b4ff));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snake_head5.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);

    setOriginMiddle(headSprite);
        break;}
    case 5:
    {nodeShape.setFillColor(sf::Color(0xFFFFF0FF));
    nodeMiddle.setFillColor(sf::Color(0x4169E1FF));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snake_head6.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);

    setOriginMiddle(headSprite);
        break;}
}
}














