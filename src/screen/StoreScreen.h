#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"

#include "element/Button.h"


namespace sfSnake
{
    class StoreScreen : public Screen
    {
    public:
        StoreScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

        // 皮肤数据结构
        struct SkinInfo
        {
            std::string name;
            int price;
            bool owned;
            sf::Texture texture;
            sf::Sprite sprite;
            sf::RectangleShape frame;  // 皮肤框架
            sf::Text nameText;         // 皮肤名称
            sf::Text priceText;        // 价格文本
        };

        // 静态数据管理
        static int playerCoins;//金币数
        static std::vector<bool> ownedSkins;  // 6个皮肤的拥有状态
        static int selectedSkin;              // 当前选中的皮肤ID (0-5)
        
        // 静态方法
        static void AddCoins(int amount);
        static void SaveGameData();
        static void LoadGameData();
        static std::string GetSelectedSkinTexture();
        static void InitializeGameData();  // 初始化游戏数据

    private:
        Button returnbutton_;
        sf::Text text_;
        sf::Text coinsText_;              // 显示金币数量
        
        // 背景
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        
        // 皮肤相关
        std::vector<SkinInfo> skins_;
        
        // UI 相关
        void InitializeSkins();
        void HandleSkinClick(int skinId);
        void PurchaseSkin(int skinId);
        bool CanAffordSkin(int skinId) const;
        void UpdateSkinDisplay();
    };
}