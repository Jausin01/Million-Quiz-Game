#include "LoginScreen.h"
#include "Config.h"
#include "UiHelpers.h"

namespace
{
    constexpr size_t kMaxNameLength = 16;
    constexpr float kErrorSeconds = 2.f;

    enum class Step
    {
        ChooseOption,
        TypeName,
    };

    const char kChoosePrompt[] = "Do you want to create a profile or\nuse one already created?";
    const char kCreatePrompt[] = "Choose a name (max 16 chars)\nPress Enter to confirm";
    const char kSignInPrompt[] = "Sign In: type your name\nPress Enter to attempt sign-in";
}

std::optional<Player> RunLoginScreen(GameContext& context)
{
    sf::RenderWindow& window = context.window;
    ResourceManager& resources = context.resources;

    sf::Sprite panel = resources.LoadSprite(config::ImageFile("MenuOptions.png"), { 1.5f, 1.5f });
    panel.setPosition({ 900.f, 200.f });
    Button createButton(resources, config::ImageFile("Buttons.png"), "Create a Player", { 650.f, 600.f });
    Button signInButton(resources, config::ImageFile("Buttons.png"), "Sign In", { 1450.f, 600.f });

    sf::Text promptText = resources.CreateText(kChoosePrompt, 30, sf::Color::Black);
    sf::Text errorText = resources.CreateText("Sign in failed - continuing as Guest", 22, sf::Color::Red);
    errorText.setPosition({ panel.getPosition().x + 40.f, panel.getPosition().y + 600.f });
    CenterTextInBox(promptText, panel);

    Step step = Step::ChooseOption;
    bool signingIn = false;
    std::string typedName;
    std::optional<sf::Clock> errorClock; // Running while the "sign in failed" message is shown
    MouseInput mouse;

    /// Text inside the panel: the question, or the instructions followed by what was typed so far.
    auto refreshPrompt = [&]()
    {
        if (step == Step::ChooseOption)
        {
            promptText.setString(kChoosePrompt);
        }
        else
        {
            promptText.setString(std::string(signingIn ? kSignInPrompt : kCreatePrompt) + "\n> " + typedName);
        }
        CenterTextInBox(promptText, panel);
    };

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                return std::nullopt;
            }
            if (step != Step::TypeName || errorClock)
            {
                continue;
            }

            if (const auto* typed = event->getIf<sf::Event::TextEntered>())
            {
                char c = typed->unicode < 128 ? (char)typed->unicode : 0;
                if (IsValidPlayerNameChar(c) && typedName.size() < kMaxNameLength)
                {
                    typedName.push_back(c);
                    refreshPrompt();
                }
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Backspace && !typedName.empty())
                {
                    typedName.pop_back();
                    refreshPrompt();
                }
                else if (key->code == sf::Keyboard::Key::Enter && !typedName.empty())
                {
                    std::optional<Player> existing = context.players.Find(typedName);
                    if (signingIn && !existing)
                    {
                        errorClock.emplace();
                    }
                    else if (existing)
                    {
                        // Signing in, or creating a name that already exists: keep the saved profile.
                        return *existing;
                    }
                    else
                    {
                        Player created(typedName);
                        context.players.Save(created);
                        return created;
                    }
                }
            }
        }

        mouse.Update(window);
        const sf::Vector2f mousePos = mouse.Position();
        bool hoverCreate = createButton.UpdateHover(mousePos);
        bool hoverSignIn = signInButton.UpdateHover(mousePos);

        if (step == Step::ChooseOption && mouse.JustPressed() && (hoverCreate || hoverSignIn))
        {
            step = Step::TypeName;
            signingIn = hoverSignIn;
            typedName.clear();
            refreshPrompt();
        }

        window.clear();
        window.draw(panel);
        window.draw(promptText);
        createButton.Draw(window);
        signInButton.Draw(window);
        if (errorClock)
        {
            window.draw(errorText);
            if (errorClock->getElapsedTime().asSeconds() >= kErrorSeconds)
            {
                return Player("Guest");
            }
        }

        mouse.EndFrame();
        window.display();
    }

    return std::nullopt;
}
