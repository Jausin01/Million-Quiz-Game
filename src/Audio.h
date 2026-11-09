#pragma once
#include <SFML/Audio.hpp>
#include <optional>

/// Music and sound effects. Every file is optional: a missing file is reported
/// once and that sound is simply skipped.
class Audio
{
public:
    Audio();

    void PlayMenuMusic();
    void StopMenuMusic();
    void PlayQuizMusic();
    void StopQuizMusic();
    void PlayRightAnswer();

    /// Wrong answer or time out: stops the quiz music and plays the game over sound.
    void PlayGameOver();

private:
    sf::Music _menuMusic;
    sf::Music _quizMusic;
    bool _hasMenuMusic = false;
    bool _hasQuizMusic = false;

    sf::SoundBuffer _rightAnswerBuffer;
    sf::SoundBuffer _gameOverBuffer;
    std::optional<sf::Sound> _rightAnswer;
    std::optional<sf::Sound> _gameOver;
};
