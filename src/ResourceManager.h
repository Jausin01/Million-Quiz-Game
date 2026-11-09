#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

/// Loads each texture and font once and hands out sprites and texts that use them.
/// Textures and fonts live as long as the manager, so the sprites and texts stay valid.
class ResourceManager
{
public:
    /// Sprite with the given scale. A missing image is reported once and
    /// replaced by an empty texture, so the game still starts.
    sf::Sprite LoadSprite(const std::string& filename, sf::Vector2f scale = { 1.f, 1.f });

    /// The game font. Throws std::runtime_error when the font file cannot be read.
    const sf::Font& GetFont();

    sf::Text CreateText(const std::string& string, unsigned characterSize = 30,
        sf::Color color = sf::Color::White);

private:
    const sf::Texture& LoadTexture(const std::string& filename);

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;
    std::unique_ptr<sf::Font> _font;
};
