#include <string>
#include <windows.h>
#include <iostream>
#include <fstream>

#include "Game.h"

#include "screen/StoreScreen.h"
#include "screen/MenuScreen.h"

namespace sfSnake {

    std::string WStringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], size_needed, NULL, NULL);
    return result;}

// 静态成员初始化
int StoreScreen::playerCoins = 0;
std::vector<bool> StoreScreen::ownedSkins = std::vector<bool>(6, false);
int StoreScreen::selectedSkin = 0;

StoreScreen::StoreScreen() 
{
    // 初始化背景
    if(!backgroundTexture_.loadFromFile("assets/image/menubg.png"))
    {
        std::cerr << "Failed to load background texture" << std::endl;
    }
    backgroundSprite_.setTexture(backgroundTexture_);
    
    // 商店标题
    text_.setFont(Game::GlobalFont);
    text_.setString(L"皮肤商店");
    text_.setCharacterSize(60);
    text_.setFillColor(sf::Color::Yellow);
    text_.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = text_.getLocalBounds();
    text_.setOrigin(textBounds.left + textBounds.width / 2.0f,
                    textBounds.top + textBounds.height / 2.0f);
    text_.setPosition(Game::GlobalVideoMode.width / 2.f,
                      Game::GlobalVideoMode.height / 8.f);

    // 金币显示
    coinsText_.setFont(Game::GlobalFont);
    coinsText_.setCharacterSize(40);
    coinsText_.setFillColor(sf::Color::Yellow);
    coinsText_.setStyle(sf::Text::Bold);
    
    // 返回按钮
    if (!returnbutton_.update("assets/image/return.png", 170.f)) {
        std::cerr << "StoreScreen: Failed to load return.png for returnButton_" << std::endl;
    }
    returnbutton_.setPosition(Game::GlobalVideoMode.width / 15.0f, 
                             Game::GlobalVideoMode.width / 15.0f);
    
    // 初始化皮肤
    InitializeSkins();
    
    // 加载游戏数据
    LoadGameData();
    
    // 更新显示
    UpdateSkinDisplay();
}

void StoreScreen::InitializeSkins()
{
    skins_.resize(6);
    
    // 皮肤信息设置
    std::vector<std::wstring> skinNames = {
        L"默认皮肤", L"吸血鬼", L"南瓜", 
        L"emoji", L"玉兔", L"阿根廷"
    };
    std::vector<int> skinPrices = {100, 200, 300, 500, 1000, 2000};
    std::vector<std::string> skinTextures = {
        "assets/image/snake_1.png",
        "assets/image/snake_2.png", 
        "assets/image/snake_3.png",
        "assets/image/snake_4.png",
        "assets/image/snake_5.png",
        "assets/image/snake_6.png"
    };
    
   
    float startX = Game::GlobalVideoMode.width * 0.2f;
    float startY = Game::GlobalVideoMode.height * 0.3f;
    float skinWidth = 200.f;
    float skinHeight = 200.f;
    float spacingX = Game::GlobalVideoMode.width * 0.25f;
    float spacingY = Game::GlobalVideoMode.height * 0.25f;

    

    for (int i = 0; i < 6; i++)
    {
        skins_[i].name = WStringToUtf8(skinNames[i]);
        skins_[i].price = skinPrices[i];
        skins_[i].owned = (i == 0); // 默认拥有第一个皮肤
        
        // 加载皮肤纹理
        if (!skins_[i].texture.loadFromFile(skinTextures[i]))
        {
            std::cerr << "Failed to load skin texture: " << skinTextures[i] << std::endl;
            // 创建默认颜色纹理
            sf::Image defaultImage;
            defaultImage.create(64, 64, sf::Color(100 + i * 25, 150, 200 - i * 20));
            skins_[i].texture.loadFromImage(defaultImage);
        }
        
        // 设置精灵
        skins_[i].sprite.setTexture(skins_[i].texture);
        float scaleX = skinWidth * 0.7f / skins_[i].texture.getSize().x;
        float scaleY = skinHeight * 0.7f / skins_[i].texture.getSize().y;
        skins_[i].sprite.setScale(scaleX, scaleY);
        
        // 计算位置 (2行3列布局)
        int row = i / 3;
        int col = i % 3;
        float posX = startX + col * spacingX;
        float posY = startY + row * spacingY;
        
        skins_[i].sprite.setPosition(posX + skinWidth * 0.15f, posY + skinHeight * 0.1f);
        
        // 设置框架
        skins_[i].frame.setSize(sf::Vector2f(skinWidth, skinHeight));
        skins_[i].frame.setPosition(posX, posY);
        skins_[i].frame.setFillColor(sf::Color(50, 50, 50, 180));
        skins_[i].frame.setOutlineThickness(3.f);
        
        // 设置名称文本
        skins_[i].nameText.setFont(Game::GlobalFont);
        skins_[i].nameText.setString(skinNames[i]);
        skins_[i].nameText.setCharacterSize(24);
        skins_[i].nameText.setFillColor(sf::Color::White);
        sf::FloatRect nameBounds = skins_[i].nameText.getLocalBounds();
        skins_[i].nameText.setOrigin(nameBounds.left + nameBounds.width / 2.0f,
                                    nameBounds.top + nameBounds.height / 2.0f);
        skins_[i].nameText.setPosition(posX + skinWidth / 2.f, posY + skinHeight * 0.8f);
        
        // 设置价格文本
        skins_[i].priceText.setFont(Game::GlobalFont);
        skins_[i].priceText.setCharacterSize(20);
        skins_[i].priceText.setFillColor(sf::Color::Yellow);
        sf::FloatRect priceBounds = skins_[i].priceText.getLocalBounds();
        skins_[i].priceText.setOrigin(priceBounds.left + priceBounds.width / 2.0f,
                                     priceBounds.top + priceBounds.height / 2.0f);
        skins_[i].priceText.setPosition(posX + skinWidth / 3.f, posY + skinHeight * 0.85f);
    }
}

void StoreScreen::UpdateSkinDisplay()
{
    // 更新金币显示
    coinsText_.setString(L"金币: " + std::to_wstring(playerCoins));
    sf::FloatRect coinsBounds = coinsText_.getLocalBounds();
    coinsText_.setOrigin(coinsBounds.left + coinsBounds.width / 2.0f,
                        coinsBounds.top + coinsBounds.height / 2.0f);
    coinsText_.setPosition(Game::GlobalVideoMode.width / 2.f,
                          Game::GlobalVideoMode.height / 5.f);
    
    // 更新每个皮肤的显示状态
    for (int i = 0; i < 6; i++)
    {
        if (ownedSkins[i])
        {
            if (i == selectedSkin)
            {
                // 当前选中的皮肤 - 绿色边框
                skins_[i].frame.setOutlineColor(sf::Color::Green);
                skins_[i].priceText.setString(L"已选中");
                skins_[i].priceText.setFillColor(sf::Color::Green);
            }
            else
            {
                // 已拥有但未选中 - 蓝色边框
                skins_[i].frame.setOutlineColor(sf::Color::Blue);
                skins_[i].priceText.setString(L"已拥有");
                skins_[i].priceText.setFillColor(sf::Color::Blue);
            }
        }
        else
        {
            // 未拥有 - 白色边框，显示价格
            skins_[i].frame.setOutlineColor(sf::Color::White);
            skins_[i].priceText.setString(std::to_wstring(skins_[i].price) + L" 金币");
            skins_[i].priceText.setFillColor(CanAffordSkin(i) ? sf::Color::Yellow : sf::Color::Red);
        }
    }
}

void StoreScreen::handleInput(sf::RenderWindow& window) 
{
    static sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    mousePosition = sf::Mouse::getPosition(window);
    returnbutton_.focused(false);
    
    // 返回按钮处理
    if (returnbutton_.contain(mousePosition))
    {
        returnbutton_.focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            SaveGameData(); // 保存数据
            Game::MainScreen = std::make_shared<MenuScreen>();
            Game::TmpScreen = nullptr;
            return;
        }
    }
    
    // 皮肤点击处理
    if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        for (int i = 0; i < 6; i++)
        {
            sf::FloatRect skinBounds = skins_[i].frame.getGlobalBounds();
            if (skinBounds.contains(static_cast<sf::Vector2f>(mousePosition)))
            {
                Game::mouseButtonCDtime = sf::Time::Zero;
                Game::mouseButtonLocked = true;
                HandleSkinClick(i);
                UpdateSkinDisplay();
                break;
            }
        }
    }
}

void StoreScreen::HandleSkinClick(int skinId)
{
    if (ownedSkins[skinId])
    {
        // 如果已拥有，则选择该皮肤
        selectedSkin = skinId;
        std::cout << "选择了皮肤: " << skins_[skinId].name << std::endl;
    }
    else
    {
        // 如果未拥有，尝试购买
        if (CanAffordSkin(skinId))
        {
            PurchaseSkin(skinId);
        }
        else
        {
            std::cout << "金币不足，无法购买皮肤: " << skins_[skinId].name << std::endl;
        }
    }
}

void StoreScreen::PurchaseSkin(int skinId)
{
    if (skinId >= 0 && skinId < 6 && !ownedSkins[skinId] && CanAffordSkin(skinId))
    {
        playerCoins -= skins_[skinId].price;
        ownedSkins[skinId] = true;
        selectedSkin = skinId; // 购买后自动选择
        std::cout << "成功购买皮肤: " << skins_[skinId].name << std::endl;
    }
}

bool StoreScreen::CanAffordSkin(int skinId) const
{
    return skinId >= 0 && skinId < 6 && playerCoins >= skins_[skinId].price;
}

void StoreScreen::update(sf::Time delta) 
{
    (void)delta; 
}

void StoreScreen::render(sf::RenderWindow& window) 
{
    window.draw(backgroundSprite_);
    window.setView(window.getDefaultView());
    
    window.draw(text_);
    window.draw(coinsText_);
    returnbutton_.render(window);
    
    // 绘制所有皮肤
    for (int i = 0; i < 6; i++)
    {
        window.draw(skins_[i].frame);
        window.draw(skins_[i].sprite);
        window.draw(skins_[i].nameText);
        window.draw(skins_[i].priceText);
    }
}

void StoreScreen::AddCoins(int amount)
{
    playerCoins += amount;
    SaveGameData();
}


void StoreScreen::SaveGameData()
{
    std::ofstream file("game_data.txt"); 
    if (file.is_open())
    {
        file << playerCoins << std::endl;   
        file << selectedSkin << std::endl;
        for (size_t i = 0; i < ownedSkins.size(); ++i)
        {
            file << (ownedSkins[i] ? 1 : 0) << (i == ownedSkins.size() - 1 ? "" : " "); 
        }
        file << std::endl; 
        file.close();
        std::cout << "Game data saved (text). Coins: " << playerCoins << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not save game data (text)." << std::endl;
    }
}



void StoreScreen::LoadGameData()
{
    std::ifstream file("game_data.txt"); 
    if (file.is_open())
    {
        std::cout << "Attempting to load game data (text) from game_data.txt..." << std::endl;
        if (!(file >> playerCoins)) { 
            std::cerr << "Text Load: Failed to read playerCoins." << std::endl;
            file.close(); InitializeGameData(); return;
        }
        if (!(file >> selectedSkin)) {
            std::cerr << "Text Load: Failed to read selectedSkin." << std::endl;
            file.close(); InitializeGameData(); return;
        }

        if (ownedSkins.size() != 6) { 
            std::cerr << "Text Load: ownedSkins vector not initialized to correct size before load." << std::endl;
            ownedSkins.assign(6, false); 
        }

        for (std::size_t i = 0; i < 6; i++) 
        {
            int owned_int;
            if (!(file >> owned_int)) {
                std::cerr << "Text Load: Failed to read ownedSkins value for index " << i << std::endl;
                file.close(); InitializeGameData(); return; 
            }
            if (i < ownedSkins.size()) { 
                ownedSkins[i] = (owned_int == 1);
            } else {
                std::cerr << "Text Load: Index " << i << " out of bounds for ownedSkins after read." << std::endl;
            }
        }
        file.close();
        std::cout << "Game data loaded successfully (text). Coins: " << playerCoins << std::endl;
    }
    else
    {
        std::cout << "No game data file (game_data.txt) found. Initializing new game data." << std::endl;
        InitializeGameData();
    }
}

void StoreScreen::InitializeGameData()
{
    playerCoins = 0;
    selectedSkin = 0;
    ownedSkins.assign(6, false);
    ownedSkins[0] = true; // 默认拥有第一个皮肤
}

std::string StoreScreen::GetSelectedSkinTexture()
{
    std::vector<std::string> skinTextures = {
        "assets/image/snake_head_1.png",
        "assets/image/snake_head_2.png",
        "assets/image/snake_head_3.png", 
        "assets/image/snake_head_4.png",
        "assets/image/snake_head_5.png",
        "assets/image/snake_head_6.png"
    };
    
    if (selectedSkin >= 0 && selectedSkin < 6)
    {
        return skinTextures[selectedSkin];
    }
    return skinTextures[0]; // 默认皮肤
}

}

