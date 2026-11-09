#include "UiHelpers.h"

void CenterTextInBox(sf::Text& text, const sf::Sprite& box, sf::Vector2f offset)
{
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({ textBounds.position.x + textBounds.size.x / 2.f,
                     textBounds.position.y + textBounds.size.y / 2.f });

    sf::FloatRect boxBounds = box.getGlobalBounds();
    text.setPosition({ boxBounds.position.x + boxBounds.size.x / 2.f + offset.x,
                       boxBounds.position.y + boxBounds.size.y / 2.f + offset.y });
}

bool IsHovered(const sf::Sprite& sprite, sf::Vector2f mousePos)
{
    return sprite.getGlobalBounds().contains(mousePos);
}

void MouseInput::Update(const sf::RenderWindow& window)
{
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    _position = { (float)mouse.x, (float)mouse.y };
    _isDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (!_started)
    {
        _wasDown = _isDown;
        _started = true;
    }
}

Button::Button(ResourceManager& resources, const std::string& textureFile, const std::string& label,
    sf::Vector2f position, sf::Vector2f scale, unsigned characterSize, sf::Color labelColor)
    : _sprite(resources.LoadSprite(textureFile, scale)),
      _label(resources.CreateText(label, characterSize, labelColor)),
      _labelColor(labelColor)
{
    _sprite.setPosition(position);
    CenterTextInBox(_label, _sprite);
}

bool Button::UpdateHover(sf::Vector2f mousePos)
{
    bool hovered = IsHovered(_sprite, mousePos);
    if (!_dimmed)
    {
        _label.setFillColor(hovered ? sf::Color::Red : _labelColor);
    }
    return hovered;
}

void Button::SetLabel(const std::string& label)
{
    _label.setString(label);
    CenterTextInBox(_label, _sprite);
}

void Button::SetDimmed(bool dimmed)
{
    _dimmed = dimmed;
    _sprite.setColor(dimmed ? sf::Color(150, 150, 150, 200) : sf::Color::White);
    if (dimmed)
    {
        _label.setFillColor(sf::Color(150, 150, 150));
    }
}

void Button::Draw(sf::RenderTarget& target) const
{
    target.draw(_sprite);
    target.draw(_label);
}
