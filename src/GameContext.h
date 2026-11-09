#pragma once
#include <SFML/Graphics.hpp>
#include "Audio.h"
#include "PlayerDatabase.h"
#include "QuestionBank.h"
#include "Random.h"
#include "ResourceManager.h"

/// Everything the screens share. The objects are owned by Game.
struct GameContext
{
    sf::RenderWindow& window;
    ResourceManager& resources;
    QuestionBank& questions;
    PlayerDatabase& players;
    Random& random;
    Audio& audio;
};
