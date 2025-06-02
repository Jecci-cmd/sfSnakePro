#pragma once

#include <vector>
#include <memory>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "screen/Screen.h"

#include "element/TitleSprite.h"
#include "element/Snake.h"
#include "element/AISnake.h"

namespace sfSnake 
{
    class Game
    {
    public:
        Game();
        ~Game() = default; 

        void run();
    public:
         static const int mapWidth;    
         static const int mapHeight;
         
    

    private:
        void handleInput();
        void update(sf::Time delta);
        void render(); 
    public:
        /* Global Color settings
         * 全局颜色设置
         */
        class Color
        {
        public:
            static const sf::Color Yellow;
            static const sf::Color Green;
            static const sf::Color Background[3];
            static const sf::Color Grid[3];
            static const sf::Color NotSeleted;
            static const sf::Color Fruit[5];
        };

        static sf::VideoMode initVideoMode_();// 初始化视频模式（分辨率，颜色深度等） 分辨率低画质差

        static sf::VideoMode GlobalVideoMode; // 游戏窗口使用的分辨率设定

        static sf::Font GlobalFont; // 游戏中统一使用的字体资源
        static TitleSprite GlobalTitle;// 游戏标题的图像资源

        static std::shared_ptr<Screen> MainScreen; // 主菜单界面
        static std::shared_ptr<Screen> TmpScreen;// 临时显示用的界面
        static std::shared_ptr<Screen> TmpGameScreen;// 临时的游戏状态界面

        static int GridVisibility;// 控制网格是否可见
        static int GridColor;// 控制网格颜色的样式编号
        static int BackgroundColor;// 当前背景色编号

        static bool mouseButtonLocked; // 防止鼠标按键连击触发
        static sf::Clock mouseButtonClock;// 记录鼠标按下的时间，用于计算是否解锁
        static sf::Time mouseButtonCDtime;// 规定的冷却时间

        static bool keyboardLocked;
        static sf::Clock keyboardClock;
        static sf::Time keyboardCDtime;

        static bool ifShowedHelp;//帮助状态

    private:
        sf::RenderWindow window_;// 主窗口对象（所有渲染操作）

        sf::Time TimePerFrame_;// 帧率控制的目标时间间隔


    };

    /**
     * @brief set any shape's origin to its middle.
     * @param shape any shape
     *
     * @code {.c++}
     * setOriginMiddle(titleSprite_);
     * @endcode
     *
     * @return sf::FloatRect shapeBounds
     *
     */
    template <typename T>
    inline sf::FloatRect setOriginMiddle(T &shape) // 设置图形的中心点到正中央（居中摆放logo时会需要）
    {
        sf::FloatRect shapeBounds = shape.getLocalBounds();
        shape.setOrigin(shapeBounds.left + shapeBounds.width / 2,// 左边+宽度的一半
                        shapeBounds.top + shapeBounds.height / 2);
        return shapeBounds;
    }

    /**
     * @brief calculate the distance between two nodes
     *
     * @param node sf::Vector2
     *
     * @return double
     */
    template <typename T1, typename T2>
    inline double dis( // 计算两个点之间的距离
        sf::Vector2<T1> node1,
        sf::Vector2<T2> node2) noexcept
    {
        return std::sqrt(
            std::pow(
                (
                    static_cast<double>(node1.x) -
                    static_cast<double>(node2.x)),
                2) +
            std::pow(
                (
                    static_cast<double>(node1.y) -
                    static_cast<double>(node2.y)),
                2));
    }

    /**
     * @brief calculate the length of a vector
     *
     * @param node sf::Vector2
     *
     * @return double
     */
    template <typename T>
    inline double length(sf::Vector2<T> node) noexcept 
    {
        return std::sqrt(
            std::pow(static_cast<double>(node.x), 2) +
            std::pow(static_cast<double>(node.y), 2));
    }
}