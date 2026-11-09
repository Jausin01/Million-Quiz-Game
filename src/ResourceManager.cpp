#include "ResourceManager.h"
#include "Config.h"
#include <iostream>
#include <stdexcept>

const sf::Texture& ResourceManager::LoadTexture(const std::string& filename)
{
    auto it = _textures.find(filename);
    if (it != _textures.end())
    {
        return *it->second;
    }

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename))
    {
        std::cerr << "Could not load image: " << filename << " (using an empty image)\n";
        texture = std::make_unique<sf::Texture>();
    }
    return *_textures.emplace(filename, std::move(texture)).first->second;
}

sf::Sprite ResourceManager::LoadSprite(const std::string& filename, sf::Vector2f scale)
{
    sf::Sprite sprite(LoadTexture(filename));
    sprite.setScale(scale);
    return sprite;
}

const sf::Font& ResourceManager::GetFont()
{
    if (!_font)
    {
        auto font = std::make_unique<sf::Font>();
        if (!font->openFromFile(config::kFontFile))
        {
            throw std::runtime_error(std::string("Could not load the font ") + config::kFontFile +
                ". Run the game from the folder that contains the assets/ and data/ folders.");
        }
        _font = std::move(font);
    }
    return *_font;
}

sf::Text ResourceManager::CreateText(const std::string& string, unsigned characterSize, sf::Color color)
{
    sf::Text text(GetFont(), string, characterSize);
    text.setFillColor(color);
    return text;
}
