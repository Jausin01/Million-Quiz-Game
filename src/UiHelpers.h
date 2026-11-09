#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "ResourceManager.h"

/// Centers a text in the middle of a sprite, then shifts it by `offset`.
void CenterTextInBox(sf::Text& text, const sf::Sprite& box, sf::Vector2f offset = { 0.f, 0.f });

/// True when the mouse position is inside the sprite.
bool IsHovered(const sf::Sprite& sprite, sf::Vector2f mousePos);

/// Tracks the left mouse button once per frame, so screens can ask
/// "was it just pressed?". A button already held when a screen opens does not count as a click.
class MouseInput
{
public:
    /// Call once at the start of every frame.
    void Update(const sf::RenderWindow& window);

    /// Call once at the end of every frame.
    void EndFrame() { _wasDown = _isDown; }

    sf::Vector2f Position() const { return _position; }
    bool JustPressed() const { return _isDown && !_wasDown; }

private:
    sf::Vector2f _position;
    bool _isDown = false;
    bool _wasDown = false;
    bool _started = false;
};

/// A sprite with a centered label. The label turns red while the mouse is over it.
class Button
{
public:
    Button(ResourceManager& resources, const std::string& textureFile, const std::string& label,
        sf::Vector2f position, sf::Vector2f scale = { 1.f, 1.f },
        unsigned characterSize = 30, sf::Color labelColor = sf::Color::White);

    /// Updates the label color and returns whether the mouse is over the button.
    bool UpdateHover(sf::Vector2f mousePos);

    void SetLabel(const std::string& label);

    /// Greyed-out look for lifelines that were already used.
    void SetDimmed(bool dimmed);

    void Draw(sf::RenderTarget& target) const;

private:
    sf::Sprite _sprite;
    sf::Text _label;
    sf::Color _labelColor;
    bool _dimmed = false;
};
