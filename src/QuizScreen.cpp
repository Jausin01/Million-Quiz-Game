#include "QuizScreen.h"
#include "Config.h"
#include "UiHelpers.h"
#include <algorithm>
#include <array>
#include <vector>

namespace
{
    // ---- Layout (pixels in the 2560x1440 window) ----
    constexpr float kAnswerX = 20.f;
    constexpr std::array<float, kAnswerCount> kAnswerY = { 660.f, 800.f, 940.f, 1090.f };
    constexpr std::array<float, kAnswerCount> kPercentageY = { 700.f, 840.f, 980.f, 1130.f };
    constexpr float kPercentageX = 800.f;
    constexpr float kFriendHandX = 850.f;
    constexpr float kRightImagesX = 850.f;

    constexpr sf::Vector2f kLifelinesToggleButtonPos = { 1600.f, 1200.f };
    constexpr sf::Vector2f kFiftyFiftyButtonPos = { 1200.f, 1000.f };
    constexpr sf::Vector2f kPhoneButtonPos = { 1600.f, 1000.f };
    constexpr sf::Vector2f kAudienceButtonPos = { 2000.f, 1000.f };

    // The picture on the right changes as time runs out.
    constexpr float kMediumTimeThreshold = 40.f;
    constexpr float kLowTimeThreshold = 20.f;

    constexpr unsigned kQuestionTextSize = 26;
}

std::optional<GameResult> RunQuizScreen(GameContext& context, GameMode mode, Player& player)
{
    sf::RenderWindow& window = context.window;
    ResourceManager& resources = context.resources;
    QuizSession session(context.questions, player, mode, context.random);

    // ---- Scenery ----
    sf::Sprite background = resources.LoadSprite(config::ImageFile("GameBackground.png"), { 3.f, 3.f });
    sf::Sprite rightImageHigh = resources.LoadSprite(config::ImageFile("Image1.jpg"), { 2.f, 2.5f });
    sf::Sprite rightImageMedium = resources.LoadSprite(config::ImageFile("Image2.jpg"), { 2.f, 2.5f });
    sf::Sprite rightImageLow = resources.LoadSprite(config::ImageFile("Image3.jpg"), { 1.5f, 2.5f });
    for (sf::Sprite* image : { &rightImageHigh, &rightImageMedium, &rightImageLow })
    {
        image->setPosition({ kRightImagesX, 0.f });
    }

    sf::Sprite questionBox = resources.LoadSprite(config::ImageFile("QuestionTittle.png"));
    questionBox.setPosition({ 0.f, 350.f });
    sf::Sprite clock = resources.LoadSprite(config::ImageFile("Clock.png"), { 0.4f, 0.4f });
    clock.setPosition({ 40.f, 10.f });
    sf::Sprite friendHand = resources.LoadSprite(config::ImageFile("FriendHand.png"));

    sf::Text questionText = resources.CreateText("", kQuestionTextSize);
    sf::Text timerText = resources.CreateText("0");
    timerText.setScale({ 2.5f, 2.5f });
    timerText.setPosition({ 105.f, 60.f });
    sf::Text prizeText = resources.CreateText("", 30, sf::Color::Black);
    prizeText.setPosition({ 430.f, 1330.f });

    // The answer boxes are part of the background image; these sprites only mark where to click.
    std::vector<sf::Sprite> answerAreas;
    std::vector<sf::Text> answerTexts;
    std::vector<sf::Text> percentageTexts;
    for (int i = 0; i < kAnswerCount; ++i)
    {
        sf::Sprite area = resources.LoadSprite(config::ImageFile("Answers.png"));
        area.setPosition({ kAnswerX, kAnswerY[i] });
        answerAreas.push_back(area);
        answerTexts.push_back(resources.CreateText(""));

        sf::Text percentage = resources.CreateText("");
        percentage.setPosition({ kPercentageX, kPercentageY[i] });
        percentageTexts.push_back(percentage);
    }

    // ---- Lifelines ----
    const std::string buttonImage = config::ImageFile("Buttons.png");
    Button lifelinesToggleButton(resources, buttonImage, "Life Lines", kLifelinesToggleButtonPos);
    Button fiftyFiftyButton(resources, buttonImage, "50/50", kFiftyFiftyButtonPos);
    Button phoneButton(resources, buttonImage, "Call a friend", kPhoneButtonPos);
    Button audienceButton(resources, buttonImage, "Ask the audience", kAudienceButtonPos);

    // ---- State of the screen ----
    std::array<bool, kAnswerCount> answerHidden = {};
    bool showLifelines = false;
    bool showFriendHand = false;
    bool showAudienceVotes = false;
    sf::Clock questionClock;
    MouseInput mouse;

    /// Shows the question of the session's current level.
    auto showQuestion = [&]()
    {
        const Question& question = session.GetQuestion();
        questionText.setString(question.text);
        CenterTextInBox(questionText, questionBox);
        for (int i = 0; i < kAnswerCount; ++i)
        {
            answerTexts[i].setString(question.answers[i]);
            CenterTextInBox(answerTexts[i], answerAreas[i]);
        }
        answerHidden.fill(false);
        showFriendHand = false;
        showAudienceVotes = false;
        prizeText.setString("$" + std::to_string(session.GetLevelPrize()));
        questionClock.restart();
    };

    showQuestion();
    context.audio.PlayQuizMusic();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        if (!window.isOpen())
        {
            break;
        }

        mouse.Update(window);
        const sf::Vector2f mousePos = mouse.Position();
        const float remaining = HasTimeLimit(mode)
            ? TimeLimitSeconds(mode) - questionClock.getElapsedTime().asSeconds()
            : TimeLimitSeconds(mode);

        // ---- Time is up ----
        if (HasTimeLimit(mode) && remaining <= 0.f)
        {
            session.TimeOut();
            context.audio.PlayGameOver();
            return session.GetResult();
        }

        // ---- Hover ----
        std::array<bool, kAnswerCount> hoverAnswer = {};
        for (int i = 0; i < kAnswerCount; ++i)
        {
            hoverAnswer[i] = !answerHidden[i] && IsHovered(answerAreas[i], mousePos);
            answerTexts[i].setFillColor(hoverAnswer[i] ? sf::Color::Red : sf::Color::White);
        }
        bool hoverLifelinesToggle = lifelinesToggleButton.UpdateHover(mousePos);
        bool hoverFiftyFifty = showLifelines && fiftyFiftyButton.UpdateHover(mousePos);
        bool hoverPhone = showLifelines && phoneButton.UpdateHover(mousePos);
        bool hoverAudience = showLifelines && audienceButton.UpdateHover(mousePos);

        // ---- Clicks ----
        if (mouse.JustPressed())
        {
            for (int i = 0; i < kAnswerCount; ++i)
            {
                if (!hoverAnswer[i])
                {
                    continue;
                }

                switch (session.Submit(i))
                {
                case AnswerResult::Correct:
                    context.audio.PlayRightAnswer();
                    showQuestion();
                    break;
                case AnswerResult::Won:
                    context.audio.StopQuizMusic();
                    context.audio.PlayRightAnswer();
                    return session.GetResult();
                case AnswerResult::Wrong:
                    context.audio.PlayGameOver();
                    return session.GetResult();
                }
                break;
            }

            if (hoverLifelinesToggle)
            {
                showLifelines = !showLifelines;
            }

            if (hoverFiftyFifty)
            {
                if (auto removed = session.UseFiftyFifty())
                {
                    for (int index : *removed)
                    {
                        answerHidden[index] = true;
                        answerTexts[index].setString("");
                    }
                }
            }

            if (hoverPhone)
            {
                if (auto suggestion = session.UsePhoneAFriend())
                {
                    friendHand.setPosition({ kFriendHandX, kAnswerY[*suggestion] });
                    showFriendHand = true;
                }
            }

            if (hoverAudience)
            {
                if (auto votes = session.UseAskAudience())
                {
                    for (int i = 0; i < kAnswerCount; ++i)
                    {
                        percentageTexts[i].setString(std::to_string((*votes)[i]) + "%");
                    }
                    showAudienceVotes = true;
                }
            }
        }

        fiftyFiftyButton.SetDimmed(!session.IsLifelineAvailable(Lifeline::FiftyFifty));
        phoneButton.SetDimmed(!session.IsLifelineAvailable(Lifeline::PhoneAFriend));
        audienceButton.SetDimmed(!session.IsLifelineAvailable(Lifeline::AskAudience));

        timerText.setString(HasTimeLimit(mode) ? sf::String(std::to_string((int)std::max(remaining, 0.f)))
                                               : sf::String(U"∞"));

        // ---- Drawing ----
        window.clear();
        if (remaining >= kMediumTimeThreshold)
        {
            window.draw(rightImageHigh);
        }
        else if (remaining >= kLowTimeThreshold)
        {
            window.draw(rightImageMedium);
        }
        else
        {
            window.draw(rightImageLow);
        }

        window.draw(background);
        window.draw(questionBox);
        window.draw(questionText);
        for (const sf::Text& answer : answerTexts)
        {
            window.draw(answer);
        }
        window.draw(clock);
        lifelinesToggleButton.Draw(window);

        if (showLifelines)
        {
            fiftyFiftyButton.Draw(window);
            phoneButton.Draw(window);
            audienceButton.Draw(window);
        }
        if (showFriendHand)
        {
            window.draw(friendHand);
        }
        if (showAudienceVotes)
        {
            for (const sf::Text& percentage : percentageTexts)
            {
                window.draw(percentage);
            }
        }
        window.draw(timerText);
        window.draw(prizeText);

        mouse.EndFrame();
        window.display();
    }

    context.audio.StopQuizMusic();
    return std::nullopt;
}
