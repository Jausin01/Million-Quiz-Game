#include "GameOverScreen.h"
#include "Config.h"
#include "UiHelpers.h"

void RunGameOverScreen(GameContext& context, const Player& player, const GameResult& result)
{
    sf::RenderWindow& window = context.window;
    ResourceManager& resources = context.resources;

    sf::Sprite background = resources.LoadSprite(config::ImageFile("bluebackground.jpg"));
    sf::Sprite titleBox = resources.LoadSprite(config::ImageFile("MenuOptions.png"));
    titleBox.setPosition({ 1000.f, 100.f });
    sf::Sprite blackBar = resources.LoadSprite(config::ImageFile("blackbar.png"), { 2.f, 2.f });
    blackBar.setPosition({ 600.f, 350.f });
    Button goBackButton(resources, config::ImageFile("Buttons.png"), "Go Back To Menu", { 2000.f, 1200.f });

    sf::Text titleText = resources.CreateText(result.won ? "Congratulations!" : "Game Over!", 40, sf::Color::Red);
    sf::Text nameText = resources.CreateText(player.GetName());
    sf::Text prizeText = resources.CreateText(
        (result.won ? "YOU WON: $" : "YOU EARNED: $") + std::to_string(result.prize));
    sf::Text gamesText = resources.CreateText("Games Played: " + std::to_string(player.GetTotalGames()));

    CenterTextInBox(titleText, titleBox);
    CenterTextInBox(nameText, blackBar, { 0.f, -250.f });
    CenterTextInBox(prizeText, blackBar, { -180.f, -200.f });
    CenterTextInBox(gamesText, blackBar, { 0.f, -130.f });

    MouseInput mouse;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        mouse.Update(window);
        bool hoverGoBack = goBackButton.UpdateHover(mouse.Position());
        if (hoverGoBack && mouse.JustPressed())
        {
            return;
        }

        window.clear();
        window.draw(background);
        window.draw(titleBox);
        window.draw(titleText);
        window.draw(blackBar);
        window.draw(nameText);
        window.draw(prizeText);
        window.draw(gamesText);
        goBackButton.Draw(window);

        mouse.EndFrame();
        window.display();
    }
}
