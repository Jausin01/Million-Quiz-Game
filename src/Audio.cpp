#include "Audio.h"
#include "Config.h"
#include <iostream>

namespace
{
    constexpr float kQuizMusicVolume = 70.f;

    bool OpenMusic(sf::Music& music, const char* path)
    {
        if (!music.openFromFile(path))
        {
            std::cerr << "Sound file not found, playing without it: " << path << "\n";
            return false;
        }
        music.setLooping(true);
        return true;
    }

    std::optional<sf::Sound> LoadSound(sf::SoundBuffer& buffer, const char* path)
    {
        if (!buffer.loadFromFile(path))
        {
            std::cerr << "Sound file not found, playing without it: " << path << "\n";
            return std::nullopt;
        }
        return sf::Sound(buffer);
    }
}

Audio::Audio()
{
    _hasMenuMusic = OpenMusic(_menuMusic, config::kMenuMusicFile);
    _hasQuizMusic = OpenMusic(_quizMusic, config::kQuizMusicFile);
    if (_hasQuizMusic)
    {
        _quizMusic.setVolume(kQuizMusicVolume);
    }
    _rightAnswer = LoadSound(_rightAnswerBuffer, config::kRightAnswerSoundFile);
    _gameOver = LoadSound(_gameOverBuffer, config::kGameOverSoundFile);
}

void Audio::PlayMenuMusic()
{
    if (_hasMenuMusic && _menuMusic.getStatus() != sf::SoundSource::Status::Playing)
    {
        _menuMusic.play();
    }
}

void Audio::StopMenuMusic()
{
    if (_hasMenuMusic)
    {
        _menuMusic.stop();
    }
}

void Audio::PlayQuizMusic()
{
    if (_hasQuizMusic)
    {
        _quizMusic.play();
    }
}

void Audio::StopQuizMusic()
{
    if (_hasQuizMusic)
    {
        _quizMusic.stop();
    }
}

void Audio::PlayRightAnswer()
{
    if (_rightAnswer)
    {
        _rightAnswer->play();
    }
}

void Audio::PlayGameOver()
{
    StopQuizMusic();
    if (_gameOver)
    {
        _gameOver->play();
    }
}
