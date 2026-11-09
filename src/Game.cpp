#include "Game.h"
#include "Audio.h"
#include "Config.h"
#include "GameContext.h"
#include "GameRules.h"
#include "MainMenuScreen.h"
#include <iostream>

int Game::Run()
{
    // Questions first: without them there is nothing to play.
    QuestionBank questions;
    for (int level = 1; level <= kLevelCount; ++level)
    {
        questions.LoadLevel(level, config::QuestionFile(level));
    }
    if (!questions.HasAllLevels(kLevelCount))
    {
        std::cerr << "Some levels have no questions. Run the game from the folder that contains data/questions.\n";
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode({ config::kWindowWidth, config::kWindowHeight }), config::kWindowTitle);
    window.setFramerateLimit(config::kFrameRate);

    ResourceManager resources;
    resources.GetFont(); // Fail early (with a clear message) if the assets are missing

    PlayerDatabase players(config::kPlayerDatabaseFile);
    Random random;
    Audio audio;

    GameContext context{ window, resources, questions, players, random, audio };
    RunMainMenu(context);
    return 0;
}
