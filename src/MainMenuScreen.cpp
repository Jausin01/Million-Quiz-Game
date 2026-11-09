#include "MainMenuScreen.h"
#include "Config.h"
#include "GameFlow.h"
#include "UiHelpers.h"
#include <vector>

namespace
{
    constexpr size_t kLeaderboardRows = 9;
    constexpr float kLeaderboardTextX = 1100.f;
    constexpr float kLeaderboardFirstY = 300.f;
    constexpr float kLeaderboardRowSpacing = 100.f;

    std::string FormatLeaderboardRow(const PlayerRecord& record)
    {
        return record.name + " | HP$" + std::to_string(record.highestPrize) +
            " | LV" + std::to_string(record.mostDifficultLevel) +
            " | G" + std::to_string(record.totalGames);
    }
}

void RunMainMenu(GameContext& context)
{
    sf::RenderWindow& window = context.window;
    ResourceManager& resources = context.resources;
    const std::string buttonImage = config::ImageFile("Buttons.png");

    sf::Sprite background = resources.LoadSprite(config::ImageFile("MenuBackGround.png"));
    Button playButton(resources, buttonImage, "Play", { 2000.f, 1000.f });
    Button quitButton(resources, buttonImage, "Quit", { 2000.f, 1200.f });
    Button leaderboardButton(resources, buttonImage, "Leaderboards", { 100.f, 50.f });

    // Panel with the game modes (its title is a label, not a button).
    Button modesTitle(resources, buttonImage, "Choose Your Game Mode", { 2000.f, 50.f }, { 1.f, 1.f }, 30, sf::Color::Magenta);
    Button classicButton(resources, buttonImage, "Classic Mode", { 2000.f, 200.f });
    Button speedButton(resources, buttonImage, "Speed Mode", { 2000.f, 350.f });
    Button practiceButton(resources, buttonImage, "Practice Mode", { 2000.f, 500.f });

    sf::Sprite leaderboardPanel = resources.LoadSprite(config::ImageFile("LeaderBoards.png"));
    leaderboardPanel.setPosition({ 950.f, 100.f });
    std::vector<sf::Text> leaderboardRows;
    for (size_t row = 0; row < kLeaderboardRows; ++row)
    {
        sf::Text text = resources.CreateText("");
        text.setPosition({ kLeaderboardTextX, kLeaderboardFirstY + kLeaderboardRowSpacing * (float)row });
        leaderboardRows.push_back(text);
    }

    bool showGameModes = false;
    bool showLeaderboard = false;
    MouseInput mouse;

    auto refreshLeaderboard = [&]()
    {
        std::vector<PlayerRecord> top = context.players.TopPlayers(kLeaderboardRows);
        for (size_t row = 0; row < kLeaderboardRows; ++row)
        {
            leaderboardRows[row].setString(row < top.size() ? FormatLeaderboardRow(top[row]) : "");
        }
    };

    auto play = [&](GameMode mode)
    {
        context.audio.StopMenuMusic();
        PlayGame(context, mode);
        context.audio.PlayMenuMusic();
    };

    context.audio.PlayMenuMusic();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Enter)
                {
                    showGameModes = !showGameModes; // Enter works like the Play button
                }
            }
        }

        mouse.Update(window);
        const sf::Vector2f mousePos = mouse.Position();
        bool hoverPlay = playButton.UpdateHover(mousePos);
        bool hoverQuit = quitButton.UpdateHover(mousePos);
        bool hoverLeaderboard = leaderboardButton.UpdateHover(mousePos);
        bool hoverClassic = showGameModes && classicButton.UpdateHover(mousePos);
        bool hoverSpeed = showGameModes && speedButton.UpdateHover(mousePos);
        bool hoverPractice = showGameModes && practiceButton.UpdateHover(mousePos);

        if (mouse.JustPressed())
        {
            if (hoverPlay)
            {
                showGameModes = !showGameModes;
            }
            if (hoverLeaderboard)
            {
                showLeaderboard = !showLeaderboard;
                if (showLeaderboard)
                {
                    refreshLeaderboard();
                }
            }
            if (hoverQuit)
            {
                window.close();
            }
            if (hoverClassic)  { play(GameMode::Classic); }
            if (hoverSpeed)    { play(GameMode::Speed); }
            if (hoverPractice) { play(GameMode::Practice); }
        }

        if (!window.isOpen())
        {
            break;
        }

        window.clear();
        window.draw(background);
        playButton.Draw(window);
        quitButton.Draw(window);
        leaderboardButton.Draw(window);

        if (showGameModes)
        {
            modesTitle.Draw(window);
            classicButton.Draw(window);
            speedButton.Draw(window);
            practiceButton.Draw(window);
        }
        if (showLeaderboard)
        {
            window.draw(leaderboardPanel);
            for (const sf::Text& row : leaderboardRows)
            {
                window.draw(row);
            }
        }

        mouse.EndFrame();
        window.display();
    }
}
